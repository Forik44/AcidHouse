// Fill out your copyright notice in the Description page of Project Settings.


#include "AHBaceCharacerMovementComponent.h"
#include "../../Characters/AHBaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "../LedgeDetectorComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Curves/CurveVector.h"
#include "DrawDebugHelpers.h"

void UAHBaseCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	CachedAHBaseCharacter = Cast<AAHBaseCharacter>(GetOwner());
}

FORCEINLINE bool UAHBaseCharacterMovementComponent::IsProning() const
{
	return CachedAHBaseCharacter && CachedAHBaseCharacter->bIsProning;
}

float UAHBaseCharacterMovementComponent::GetMaxSpeed() const
{
	float Result = Super::GetMaxSpeed();

	if (IsSprinting())
	{
		Result = SprintSpeed;
	}
	else if (CachedAHBaseCharacter->bIsProning)
	{
		Result = MaxProneSpeed;
	}
	else if (IsFastSwimming())
	{
		Result = FastSwimmingSpeed;
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

	if (CachedAHBaseCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == ProneCapsuleHalfHeight)
	{
		CachedAHBaseCharacter->bIsProning = true;
		CachedAHBaseCharacter->OnStartProne(0.f, 0.f);
		return;
	}

	const float ComponentScale = CachedAHBaseCharacter->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CachedAHBaseCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = CachedAHBaseCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius();

	const float ClampedPronedHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, ProneCapsuleHalfHeight);
	CachedAHBaseCharacter->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, ClampedPronedHalfHeight);
	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedPronedHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	if (ClampedPronedHalfHeight > OldUnscaledHalfHeight)
	{
		FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, CachedAHBaseCharacter);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);
		const bool bEncroached = GetWorld()->OverlapBlockingTestByChannel(UpdatedComponent->GetComponentLocation() - FVector(0.f, 0.f, ScaledHalfHeightAdjust), FQuat::Identity,
			UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleParams, ResponseParam);

		if (bEncroached)
		{
			CachedAHBaseCharacter->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, OldUnscaledHalfHeight);
			return;
		}
	}

	if (bCrouchMaintainsBaseLocation)
	{
		UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	}
	CachedAHBaseCharacter->bIsProning = true;
	
	bForceNextFloorCheck = true;

	const float MeshAdjust = ScaledHalfHeightAdjust;
	HalfHeightAdjust = (CrouchedHalfHeight - ClampedPronedHalfHeight);
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	AdjustProxyCapsuleSize();
	CachedAHBaseCharacter->OnStartProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void UAHBaseCharacterMovementComponent::UnProne()
{
	if (!HasValidData())
	{
		return;
	}

	AAHBaseCharacter* DefaultAHBaseCharacter = CachedAHBaseCharacter->GetClass()->GetDefaultObject<AAHBaseCharacter>();

	if (CachedAHBaseCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == CrouchedHalfHeight)
	{
		CachedAHBaseCharacter->bIsProning = false;
		CachedAHBaseCharacter->OnEndProne(0.f, 0.f);
		return;
	}

	const float CurrentPronedHalfHeight = CachedAHBaseCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const float ComponentScale = CachedAHBaseCharacter->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CachedAHBaseCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float HalfHeightAdjust = CrouchedHalfHeight - OldUnscaledHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	check(CachedAHBaseCharacter->GetCapsuleComponent());


	const UWorld* MyWorld = GetWorld();
	const float SweepInflation = KINDA_SMALL_NUMBER * 10.f;
	FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, CachedAHBaseCharacter);
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
				CachedAHBaseCharacter->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);
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

	CachedAHBaseCharacter->bIsProning = false;

	CachedAHBaseCharacter->GetCapsuleComponent()->SetCapsuleSize(DefaultAHBaseCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), CrouchedHalfHeight, true);

	const float MeshAdjust = ScaledHalfHeightAdjust;
	AdjustProxyCapsuleSize();
	CachedAHBaseCharacter->OnEndProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void UAHBaseCharacterMovementComponent::StartMantle(const FMantlingMovementParameters& MantlingParameters)
{
	CurrentMantlingParametrs = MantlingParameters;
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Mantling);
}

void UAHBaseCharacterMovementComponent::EndMantle()
{
	SetMovementMode(MOVE_Walking);
}

bool UAHBaseCharacterMovementComponent::IsMantling() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Mantling;
}

bool UAHBaseCharacterMovementComponent::CanAttemptMantle() const
{
	return !IsMantling() && !IsProning() && !IsCrouching();
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
		CachedAHBaseCharacter->bIsCrouched = false;
		CachedAHBaseCharacter->bIsProning = false;
		CachedAHBaseCharacter->OnSwimStart();
		CachedAHBaseCharacter->GetCapsuleComponent()->SetCapsuleSize(SwimmingCapsuleRadius, SwimmingCapsuleHalfHeight);
	}
	else if (PreviousMovementMode == MOVE_Swimming)
	{
		AAHBaseCharacter* DefaultCharacter = CachedAHBaseCharacter->GetClass()->GetDefaultObject<AAHBaseCharacter>();
		CachedAHBaseCharacter->OnSwimEnd();
		CachedAHBaseCharacter->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);
		CachedAHBaseCharacter->GetCapsuleComponent()->SetWorldRotation(FQuat(0, 0, 0, 0));
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
	switch (CustomMovementMode)
	{
		case (uint8)ECustomMovementMode::CMOVE_Mantling:
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

			FHitResult Hit;
			SafeMoveUpdatedComponent(Delta, NewRotation, false, Hit);
			break;
		}
		default:
			break;
	}

	Super::PhysCustom(DeltaTime, Iterations); 
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
