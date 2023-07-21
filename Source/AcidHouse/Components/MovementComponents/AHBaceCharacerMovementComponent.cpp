// Fill out your copyright notice in the Description page of Project Settings.


#include "AHBaceCharacerMovementComponent.h"
#include "Characters/AHBaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/LedgeDetectorComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Curves/CurveVector.h"
#include "DrawDebugHelpers.h"
#include "Actors/Interactive/Enviroment/Ladder.h"
#include "Actors/Interactive/Enviroment/Zipline.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAHBaseCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	CachedAHBaseCharacter = Cast<AAHBaseCharacter>(GetOwner());
}

bool UAHBaseCharacterMovementComponent::IsProning() const
{
	return GetBaseCharacterOwner() && GetBaseCharacterOwner()->bIsProning;
}

FNetworkPredictionData_Client* UAHBaseCharacterMovementComponent::GetPredictionData_Client() const 
{
	if (!ClientPredictionData)
	{
		UAHBaseCharacterMovementComponent* MutableThis = const_cast<UAHBaseCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Character_AH(*this);
	}

	return ClientPredictionData;
}

void UAHBaseCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	//	FLAG_Reserved_1 = 0x04,	// Reserved for future use
	//	FLAG_Reserved_2 = 0x08,	// Reserved for future use
	//	// Remaining bit masks are available for custom flags.
	//	FLAG_Custom_0 = 0x10, - Sprinting flag
	//	FLAG_Custom_1 = 0x20, - Mantling flag
	//	FLAG_Custom_2 = 0x40,
	//	FLAG_Custom_3 = 0x80,

	bool bWasMantling = GetBaseCharacterOwner()->bIsMantling;
	bIsSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	bool bIsMantling = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;

	if (GetBaseCharacterOwner()->GetLocalRole() == ROLE_Authority)
	{
		if (!bWasMantling && bIsMantling)
		{
			GetBaseCharacterOwner()->Mantle(true);
		}
	}
}

void UAHBaseCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	if (bForceRotation)
	{
		FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
		CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

		FRotator DeltaRot = GetDeltaRotation(DeltaTime);
		DeltaRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): GetDeltaRotation"));

		const float AngleTolerance = 1e-3f;

		if (!CurrentRotation.Equals(ForceTargetRotation, AngleTolerance))
		{
			FRotator DesiredRotation = ForceTargetRotation;
			// PITCH
			if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
			{
				DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
			}

			// YAW
			if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
			{
				DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
			}

			// ROLL
			if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
			{
				DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
			}

			// Set the new rotation.
			DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
			MoveUpdatedComponent(FVector::ZeroVector, DesiredRotation, /*bSweep*/ false);
		}
		else
		{
			ForceTargetRotation = FRotator::ZeroRotator;
			bForceRotation = false;
		}

		return;
	}
	if (IsOnLadder())
	{
		return;
	}
	Super::PhysicsRotation(DeltaTime);
}

float UAHBaseCharacterMovementComponent::GetMaxSpeed() const
{
	float Result = Super::GetMaxSpeed();

	if (!IsValid(GetBaseCharacterOwner()))
	{
		return Result;
	}

	if (IsSprinting())
	{
		Result = SprintSpeed;
	}
	else if (IsProning())
	{
		Result = MaxProneSpeed;
	}
	else if (IsFastSwimming())
	{
		Result = FastSwimmingSpeed;
	}
	else if (IsOnLadder())
	{
		Result = ClimbingOnLadderMaxSpeed;
		return Result;
	}
	else if (IsOnZipline())
	{
		Result = ZiplineMaxSpeed;
		return Result;
	}
	else if (GetBaseCharacterOwner()->IsAiming())
	{
		Result = GetBaseCharacterOwner()->GetAimingMovementSpeed();
	}
	
	if (bIsOutOfStamina)
	{
		Result = OutOfStaminaSpeed;
	}

	return Result;
}

void UAHBaseCharacterMovementComponent::StartSprint()
{
	bIsSprinting = true;
	bForceMaxAccel = 1;
}

void UAHBaseCharacterMovementComponent::StopSprint()
{
	bIsSprinting = false;
	bForceMaxAccel = 0;
}

void UAHBaseCharacterMovementComponent::StartFastSwim()
{
	bIsFastSwimming = true;
}

void UAHBaseCharacterMovementComponent::StopFastSwim()
{
	bIsFastSwimming = false;
}

bool UAHBaseCharacterMovementComponent::CanEverSprint()
{
	return !FMath::IsNearlyZero(GetOwner()->GetVelocity().Size(), 1e-6f) && !bIsOutOfStamina;
}

bool UAHBaseCharacterMovementComponent::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState() && !bIsOutOfStamina;
}

void UAHBaseCharacterMovementComponent::SetIsOutOfStamina(bool bIsOutOfStamina_In)
{
	bIsOutOfStamina = bIsOutOfStamina_In;
	if (bIsOutOfStamina_In)
	{
		bIsSprinting = false;
		bIsFastSwimming = false;
	}
}

bool UAHBaseCharacterMovementComponent::CanProneInCurrentState() const
{
	if (!CanEverCrouch())
	{
		return false;
	}

	return (IsFalling() || IsMovingOnGround()) && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics() && IsCrouching();
}

void UAHBaseCharacterMovementComponent::Prone()
{
	if (!HasValidData())
	{
		return;
	}

	if (GetBaseCharacterOwner()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == ProneCapsuleHalfHeight)
	{
		GetBaseCharacterOwner()->bIsProning = true;
		GetBaseCharacterOwner()->OnStartProne(0.f, 0.f);
		return;
	}

	const float ComponentScale = GetBaseCharacterOwner()->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = GetBaseCharacterOwner()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = GetBaseCharacterOwner()->GetCapsuleComponent()->GetUnscaledCapsuleRadius();

	const float ClampedPronedHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, ProneCapsuleHalfHeight);
	GetBaseCharacterOwner()->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, ClampedPronedHalfHeight);
	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedPronedHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	if (ClampedPronedHalfHeight > OldUnscaledHalfHeight)
	{
		FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, GetBaseCharacterOwner());
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);
		const bool bEncroached = GetWorld()->OverlapBlockingTestByChannel(UpdatedComponent->GetComponentLocation() - FVector(0.f, 0.f, ScaledHalfHeightAdjust), FQuat::Identity,
			UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleParams, ResponseParam);

		if (bEncroached)
		{
			GetBaseCharacterOwner()->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, OldUnscaledHalfHeight);
			return;
		}
	}

	if (bCrouchMaintainsBaseLocation)
	{
		UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	}
	GetBaseCharacterOwner()->bIsProning = true;
	
	bForceNextFloorCheck = true;

	const float MeshAdjust = ScaledHalfHeightAdjust;
	HalfHeightAdjust = (GetCrouchedHalfHeight() - ClampedPronedHalfHeight);
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	AdjustProxyCapsuleSize();
	GetBaseCharacterOwner()->OnStartProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void UAHBaseCharacterMovementComponent::UnProne()
{
	if (!HasValidData() || !IsValid(GetBaseCharacterOwner()))
	{
		return;
	}

	AAHBaseCharacter* DefaultAHBaseCharacter = GetBaseCharacterOwner()->GetClass()->GetDefaultObject<AAHBaseCharacter>();

	if (GetBaseCharacterOwner()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == GetCrouchedHalfHeight())
	{
		GetBaseCharacterOwner()->bIsProning = false;
		GetBaseCharacterOwner()->OnEndProne(0.f, 0.f);
		return;
	}

	const float CurrentPronedHalfHeight = GetBaseCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const float ComponentScale = GetBaseCharacterOwner()->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = GetBaseCharacterOwner()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float HalfHeightAdjust = GetCrouchedHalfHeight() - OldUnscaledHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	check(GetBaseCharacterOwner()->GetCapsuleComponent());


	const UWorld* MyWorld = GetWorld();
	const float SweepInflation = KINDA_SMALL_NUMBER * 10.f;
	FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, GetBaseCharacterOwner());
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(CapsuleParams, ResponseParam);

	const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust); // Shrink by negative amount, so actually grow it.
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
	bool bEncroached = true;

	if (!bProneMaintainsBaseLocation)
	{
		bEncroached = MyWorld->OverlapBlockingTestByChannel(PawnLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

		if (bEncroached)
		{
			if (ScaledHalfHeightAdjust > 0.f)
			{
				float PawnRadius, PawnHalfHeight;
				GetBaseCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);
				const float ShrinkHalfHeight = PawnHalfHeight - PawnRadius;
				const float TraceDist = PawnHalfHeight - ShrinkHalfHeight;
				const FVector Down = FVector(0.f, 0.f, -TraceDist);

				FHitResult Hit(1.f);
				const FCollisionShape ShortCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, ShrinkHalfHeight);
				const bool bBlockingHit = MyWorld->SweepSingleByChannel(Hit, PawnLocation, PawnLocation + Down, FQuat::Identity, CollisionChannel, ShortCapsuleShape, CapsuleParams);
				if (Hit.bStartPenetrating)
				{
					bEncroached = true;
				}
				else
				{
					const float DistanceToBase = (Hit.Time * TraceDist) + ShortCapsuleShape.Capsule.HalfHeight;
					const FVector NewLoc = FVector(PawnLocation.X, PawnLocation.Y, PawnLocation.Z - DistanceToBase + StandingCapsuleShape.Capsule.HalfHeight + SweepInflation + MIN_FLOOR_DIST / 2.f);
					bEncroached = MyWorld->OverlapBlockingTestByChannel(NewLoc, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
					if (!bEncroached)
					{
						UpdatedComponent->MoveComponent(NewLoc - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
					}
				}
			}
		}
	}
	else
	{
		FVector StandingLocation = PawnLocation + FVector(0.f, 0.f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentPronedHalfHeight);
		bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

		if (bEncroached)
		{
			if (IsMovingOnGround())
			{
				const float MinFloorDist = KINDA_SMALL_NUMBER * 10.f;
				if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFloorDist)
				{
					StandingLocation.Z -= CurrentFloor.FloorDist - MinFloorDist;
					bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
				}
			}
		}

		if (!bEncroached)
		{
			UpdatedComponent->MoveComponent(StandingLocation - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
			bForceNextFloorCheck = true;
		}
	}

	if (bEncroached)
	{
		return;
	}

	GetBaseCharacterOwner()->bIsProning = false;

	GetBaseCharacterOwner()->GetCapsuleComponent()->SetCapsuleSize(DefaultAHBaseCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), GetCrouchedHalfHeight(), true);

	const float MeshAdjust = ScaledHalfHeightAdjust;
	AdjustProxyCapsuleSize();
	GetBaseCharacterOwner()->OnEndProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void UAHBaseCharacterMovementComponent::StartMantle(const FMantlingMovementParameters& MantlingParameters)
{
	CurrentMantlingParametrs = MantlingParameters;
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Mantling);
}

void UAHBaseCharacterMovementComponent::EndMantle()
{
	GetBaseCharacterOwner()->bIsMantling = false;
	SetMovementMode(MOVE_Walking);
}

bool UAHBaseCharacterMovementComponent::CanAttemptMantle() const
{
	return !IsMantling() && !IsProning() && !IsCrouching() && !IsOnLadder();
}

bool UAHBaseCharacterMovementComponent::IsMantling() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Mantling;
}

bool UAHBaseCharacterMovementComponent::IsOnLadder() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Ladder;
}

float UAHBaseCharacterMovementComponent::GetLadderSpeedRation() const
{
	checkf(IsValid(CurrentLadder), TEXT("UAHBaseCharacterMovementComponent::GetLadderSpeedRation() cannot be invoked when current ladder is null"))

	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	return FVector::DotProduct(LadderUpVector, Velocity) / ClimbingOnLadderMaxSpeed;
}

bool UAHBaseCharacterMovementComponent::IsOnZipline() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Zipline;
}

void UAHBaseCharacterMovementComponent::AttachToLadder(const ALadder* Ladder)
{
	CurrentLadder = Ladder;
	Velocity = FVector::UpVector;

	FRotator TargetOrientationRotation = CurrentLadder->GetActorForwardVector().ToOrientationRotator();
	TargetOrientationRotation.Yaw += 180.0f;

	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderForwardVector = CurrentLadder->GetActorForwardVector();
	float Projection = GetActorToCurrentLadderProjection(GetActorLocation());

	FVector NewCharacterLocation = CurrentLadder->GetActorLocation() + Projection * LadderUpVector + LadderToCharacterOffset * LadderForwardVector;
	if (CurrentLadder->GetIsOnTop())
	{
		NewCharacterLocation = CurrentLadder->GetAttachFromTopAnimMontageStartingLocation();
	}

	GetOwner()->SetActorLocation(NewCharacterLocation);
	GetOwner()->SetActorRotation(TargetOrientationRotation);

	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Ladder);
}

float UAHBaseCharacterMovementComponent::GetActorToCurrentLadderProjection(const FVector& Location) const
{
	checkf(IsValid(CurrentLadder), TEXT("UAHBaseCharacterMovementComponent::GetActorToCurrentLadderProjection(const FVector& Location) cannot be invoked when current ladder is null"))

	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderToCharacterDistance = GetActorLocation() - CurrentLadder->GetActorLocation();
	return FVector::DotProduct(LadderUpVector, LadderToCharacterDistance);
}

float UAHBaseCharacterMovementComponent::GetActorToCurrentZiplineProjection(const FVector& Location) const
{
	checkf(IsValid(CurrentZipline), TEXT("UAHBaseCharacterMovementComponent::GetActorToCurrentZiplineProjection(const FVector& Location) cannot be invoked when current ladder is null"))
;
	FVector LadderToCharacterDistance = GetActorLocation() - CurrentZipline->GetActorLocation();
	return FVector::DotProduct(ZiplineDirection, LadderToCharacterDistance);
}

void UAHBaseCharacterMovementComponent::DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod /*= EDetachFromLadderMethod::Fall*/)
{
	switch (DetachFromLadderMethod)
	{
	case EDetachFromLadderMethod::Fall:
	{
		SetMovementMode(MOVE_Falling);
		break;
	}
	case EDetachFromLadderMethod::ReachingTheTop:
	{
		GetBaseCharacterOwner()->Mantle(true);
		break;
	}
	case EDetachFromLadderMethod::ReachingTheBottom:
	{
		SetMovementMode(MOVE_Walking);
		break;
	}
	case EDetachFromLadderMethod::JumpOff:
	{
		FVector JumpDirection = CurrentLadder->GetActorForwardVector();

		SetMovementMode(MOVE_Falling);

		FVector JumpVelocity = JumpDirection * JumpOffFromLadderSpeed;

		ForceTargetRotation = JumpDirection.ToOrientationRotator();
		bForceRotation = true;

		Launch(JumpVelocity);
		break;
	}
	default:
	{
		SetMovementMode(MOVE_Falling);
		break;
	}
	}
}

void UAHBaseCharacterMovementComponent::AttachToZipline(const AZipline* Zipline)
{
	CurrentZipline = Zipline;
	ZiplineDirection = -CurrentZipline->GetZiplineDirection();
	ZiplineDirection.Normalize();
	Velocity = ZiplineDirection;

	FRotator TargetOrientationRotation = CurrentZipline->GetZiplineDirection().ToOrientationRotator();
	TargetOrientationRotation.Yaw += 180.0f;

	FVector ZiplineUpVector = CurrentZipline->GetActorUpVector();
	FVector ZiplineForwardVector = CurrentZipline->GetActorForwardVector();
	float Projection = GetActorToCurrentZiplineProjection(GetActorLocation());

	FVector NewCharacterLocation = CurrentZipline->GetActorLocation() + CurrentZipline->GetHeight() * FVector::UpVector + Projection * ZiplineDirection - ZiplineToCharacterOffset * ZiplineUpVector;

	GetOwner()->SetActorLocation(NewCharacterLocation);
	GetOwner()->SetActorRotation(TargetOrientationRotation);

	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Zipline);
}

void UAHBaseCharacterMovementComponent::DetachFromZipline()
{
	SetMovementMode(MOVE_Falling);
}

void UAHBaseCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviusCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviusCustomMode);

	StopSprint();
	if (PreviousMovementMode != MOVE_Falling && MovementMode == MOVE_Falling)
	{
		UnProne();
		UnCrouch();
	}

	if (MovementMode == MOVE_Swimming)
	{
		GetBaseCharacterOwner()->bIsCrouched = false;
		GetBaseCharacterOwner()->bIsProning = false;
		GetBaseCharacterOwner()->OnSwimStart();
		GetBaseCharacterOwner()->GetCapsuleComponent()->SetCapsuleSize(SwimmingCapsuleRadius, SwimmingCapsuleHalfHeight);
	}
	else if (PreviousMovementMode == MOVE_Swimming)
	{
		AAHBaseCharacter* DefaultCharacter = GetBaseCharacterOwner()->GetClass()->GetDefaultObject<AAHBaseCharacter>();
		GetBaseCharacterOwner()->OnSwimEnd();
		GetBaseCharacterOwner()->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);
		GetBaseCharacterOwner()->GetCapsuleComponent()->SetWorldRotation(FQuat(0, 0, 0, 0));
	}

	if (PreviousMovementMode == MOVE_Custom && PreviusCustomMode == (uint8)ECustomMovementMode::CMOVE_Ladder)
	{
		CurrentLadder = nullptr;
	}

	if (PreviousMovementMode == MOVE_Custom && PreviusCustomMode == (uint8)ECustomMovementMode::CMOVE_Zipline)
	{
		CurrentZipline = nullptr;
	}

	if (MovementMode == MOVE_Custom)
	{
		switch (CustomMovementMode)
		{
			case (uint8)ECustomMovementMode::CMOVE_Mantling:
			{

				GetWorld()->GetTimerManager().SetTimer(MantlingTimer, this, &UAHBaseCharacterMovementComponent::EndMantle, CurrentMantlingParametrs.Duration, false);
				break;
			}

			default: 
				break;
		}
	}
}

void UAHBaseCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	if (GetBaseCharacterOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		return;
	}

	switch (CustomMovementMode)
	{
		case (uint8)ECustomMovementMode::CMOVE_Mantling:
		{
			PhysMantling(DeltaTime, Iterations);
			break;
		}
		case (uint8)ECustomMovementMode::CMOVE_Ladder:
		{
			PhysLadder(DeltaTime, Iterations);
			break; 
		}
		case (uint8)ECustomMovementMode::CMOVE_Zipline:
		{
			PhysZipline(DeltaTime, Iterations);
			break;
		}
		default:
			break;
	}

	Super::PhysCustom(DeltaTime, Iterations); 
}

void UAHBaseCharacterMovementComponent::PhysMantling(float DeltaTime, int32 Iterations)
{
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(MantlingTimer) + CurrentMantlingParametrs.StartTime;

	FVector MantlingCurveValue = CurrentMantlingParametrs.MantlingCurve->GetVectorValue(ElapsedTime);

	float PositionAlpha = MantlingCurveValue.X;
	float XYCorrectionAlpha = MantlingCurveValue.Y;
	float ZCorrectionAlpha = MantlingCurveValue.Z;

	FVector CorrectedInitialLocation = FMath::Lerp(CurrentMantlingParametrs.InitialLocation, CurrentMantlingParametrs.InitialAimationLocation, XYCorrectionAlpha);
	CorrectedInitialLocation.Z = FMath::Lerp(CurrentMantlingParametrs.InitialLocation.Z, CurrentMantlingParametrs.InitialAimationLocation.Z, ZCorrectionAlpha);

	FVector NewTargetOffset = CurrentMantlingParametrs.LedgeActor->GetActorLocation() - CurrentMantlingParametrs.TargetLocation;

	CurrentMantlingParametrs.TargetLocation += NewTargetOffset - CurrentMantlingParametrs.TargetOffset;

	FVector NewLocation = FMath::Lerp(CorrectedInitialLocation, CurrentMantlingParametrs.TargetLocation, PositionAlpha);
	FRotator NewRotation = FMath::Lerp(CurrentMantlingParametrs.InitialRotation, CurrentMantlingParametrs.TargetRotation, PositionAlpha);

	FVector Delta = NewLocation - GetActorLocation();
	Velocity = Delta / DeltaTime;

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, NewRotation, false, Hit);
}

void UAHBaseCharacterMovementComponent::PhysLadder(float DeltaTime, int32 Iterations)
{
	CalcVelocity(DeltaTime, 1.0f, false, ClimbingOnLadderBreakingDecelaration);
	FVector Delta = Velocity * DeltaTime;

	if (HasAnimRootMotion())
	{
		FHitResult Hit;
		FRotator TargetOrientationRotation = CurrentLadder->GetActorForwardVector().ToOrientationRotator();
		TargetOrientationRotation.Yaw += 180.0f;
		SafeMoveUpdatedComponent(Delta, TargetOrientationRotation, false, Hit);
		return;
	}

	FVector NewPosition = GetActorLocation() + Delta;
	float NewPositionProjection = GetActorToCurrentLadderProjection(NewPosition);

	if (NewPositionProjection < MinLadderBottomOffset)
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheBottom);
		return;
	}
	else if (NewPositionProjection > (CurrentLadder->GetLadderHeight() - MaxLadderTopOffset))
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheTop);
		return;
	}

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
}

void UAHBaseCharacterMovementComponent::PhysZipline(float DeltaTime, int32 Iterations)
{
	AddInputVector(ZiplineDirection);
	CalcVelocity(DeltaTime, 1.0f, false, ClimbingOnLadderBreakingDecelaration);
	FVector Delta = Velocity * DeltaTime;

	FVector NewPosition = GetActorLocation() + Delta;
	float NewPositionProjection = GetActorToCurrentZiplineProjection(NewPosition);

	if (NewPositionProjection > CurrentZipline->GetCableLength())
	{
		DetachFromZipline();
		return;
	}

	if (!IsValid(CurrentZipline))
	{
		DetachFromZipline();
		return;
	}

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
}

bool UAHBaseCharacterMovementComponent::CanAttemptJump() const
{
	return Super::CanAttemptJump() && !bIsOutOfStamina;
}

void UAHBaseCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	if (GetOwner()->GetLocalRole() != ROLE_SimulatedProxy)
	{
		const bool bIsCrouching = IsCrouching();
		const bool bIsProning = IsProning();

		if (bIsProning && (!bWantsToProne || !CanProneInCurrentState()))
		{
			UnProne();
		}
		else if (bIsCrouching && (!bWantsToCrouch || !CanCrouchInCurrentState()) && !bWantsToProne)
		{
			UnCrouch(false);
		}
		else if (bIsCrouching && bWantsToProne && CanProneInCurrentState() && !bIsProning)
		{
			Prone();
		}
		else if (!bIsCrouching && bWantsToCrouch && CanCrouchInCurrentState())
		{
			Crouch(false);
		}
	}

}

void FSavedMove_AH::Clear()
{
	Super::Clear();
	bSavedIsSprinting = 0;
	bSavedIsMantling = 0;
}

uint8 FSavedMove_AH::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	//	FLAG_Reserved_1 = 0x04,	// Reserved for future use
	//	FLAG_Reserved_2 = 0x08,	// Reserved for future use
	//	// Remaining bit masks are available for custom flags.
	//	FLAG_Custom_0 = 0x10, - Sprinting flag
	//	FLAG_Custom_1 = 0x20, - Mantling flag
	//	FLAG_Custom_2 = 0x40,
	//	FLAG_Custom_3 = 0x80,

	if (bSavedIsSprinting)
	{
		Result |= FLAG_Custom_0;
	}

	if (bSavedIsMantling)
	{
		Result &= ~FLAG_JumpPressed;
		Result |= FLAG_Custom_1;
	}

	return Result;
}

bool FSavedMove_AH::CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const
{
	const FSavedMove_AH* NewMove = StaticCast<const FSavedMove_AH*>(NewMovePtr.Get());

	if (bSavedIsSprinting != NewMove->bSavedIsSprinting || bSavedIsMantling != NewMove->bSavedIsMantling)
	{
		return false;
	}

	return Super::CanCombineWith(NewMovePtr, InCharacter, MaxDelta);
}

void FSavedMove_AH::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	checkf(C->IsA<AAHBaseCharacter>(), TEXT("FSavedMove_AH::SetMoveFor working only with AAHBaseCharacter"));
	AAHBaseCharacter* InBaseCharacter = StaticCast<AAHBaseCharacter*>(C);
	UAHBaseCharacterMovementComponent* MovementComponent = InBaseCharacter->GetBaseCharacterMovementComponent();

	bSavedIsSprinting = MovementComponent->bIsSprinting;
	bSavedIsMantling = InBaseCharacter->bIsMantling;
}

void FSavedMove_AH::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	UAHBaseCharacterMovementComponent* MovementComponent = StaticCast<UAHBaseCharacterMovementComponent*>(C->GetMovementComponent());

	MovementComponent->bIsSprinting = bSavedIsSprinting;
}

FNetworkPredictionData_Client_Character_AH::FNetworkPredictionData_Client_Character_AH(const UAHBaseCharacterMovementComponent& ClientMovement) 
	:
	Super(ClientMovement)
{

}

FSavedMovePtr FNetworkPredictionData_Client_Character_AH::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_AH());
}
