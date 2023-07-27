#include "AHBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/MovementComponents/AHBaseCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/LedgeDetectorComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"
#include "Actors/Interactive/InteractiveActor.h"
#include "Actors/Interactive/Enviroment/Ladder.h"
#include "Actors/Interactive/Enviroment/Zipline.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Curves/CurveVector.h"
#include "AcidHouseTypes.h"
#include "Actors/Equipment/Weapon/RangeWeapon.h"
#include "Actors/Equipment/Weapon/MeleeWeapon.h"
#include "AIController.h"
#include "Net/UnrealNetwork.h"
#include "Actors/Interactive/Interface/IInteractable.h"


AAHBaseCharacter::AAHBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: 
	Super(ObjectInitializer.SetDefaultSubobjectClass<UAHBaseCharacterMovementComponent> (ACharacter::CharacterMovementComponentName))
{
	AHBaseCharacterMovementComponent = StaticCast<UAHBaseCharacterMovementComponent*>(GetCharacterMovement());

	IKTraceDistance = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("LedgeDetector"));

	GetMesh()->CastShadow = true;
	GetMesh()->bCastDynamicShadow = true;

	CharacterAttributeComponent = CreateDefaultSubobject<UCharacterAttributeComponent>(TEXT("CharacterAttributes"));
	CharacterEquipmentComponent = CreateDefaultSubobject<UCharacterEquipmentComponent>(TEXT("CharacterEquipment"));
}

void AAHBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	CharacterAttributeComponent->OnDeathEvent.AddUObject(this, &AAHBaseCharacter::OnDeath);
	CharacterAttributeComponent->OnOutOfStaminaEvent.AddDynamic(this, &AAHBaseCharacter::OnOutOfStamina);
	CharacterAttributeComponent->OnOutOfOxygenEvent.AddDynamic(this, &AAHBaseCharacter::OnOutOfOxygen);
}

void AAHBaseCharacter::TryJump()
{
	if (CanJump() && !IsMeleeAttacking())
	{
		Jump();
		return;
	} 
	
	if(GetBaseCharacterMovementComponent()->IsProning())
	{
		UnProne();
	}
	
	if (GetBaseCharacterMovementComponent()->IsCrouching())
	{
		UnCrouch();
	}
}

void AAHBaseCharacter::ChangeCrouchState()
{
	if (GetBaseCharacterMovementComponent()->IsCrouching() && !GetBaseCharacterMovementComponent()->CanProneInCurrentState())
	{
		UnCrouch();
	}
	else if (!GetBaseCharacterMovementComponent()->IsCrouching())
	{
		Crouch();
	}
}

void AAHBaseCharacter::ChangeProneState()
{
	if (GetBaseCharacterMovementComponent()->IsProning())
	{
		UnProne();
	}
	else if (GetBaseCharacterMovementComponent()->IsCrouching())
	{
		Prone();
	}
}

void AAHBaseCharacter::Prone()
{
	if (GetBaseCharacterMovementComponent())
	{
		if (CanProne())
		{
			GetBaseCharacterMovementComponent()->bWantsToProne = true;
		}
	}
}

void AAHBaseCharacter::UnProne()
{
	if (GetBaseCharacterMovementComponent())
	{
		GetBaseCharacterMovementComponent()->bWantsToProne = false;
	}
}

void AAHBaseCharacter::StartSprint()
{
	bIsSprintRequested = true;
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

void AAHBaseCharacter::StopSprint()
{
	bIsSprintRequested = false;
}

void AAHBaseCharacter::StartFastSwim()
{
	bIsFastSwimRequested = true;
}

void AAHBaseCharacter::StopFastSwim()
{
	bIsFastSwimRequested = false;
}

void AAHBaseCharacter::OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	RecalculateBaseEyeHeight();

	USkeletalMeshComponent* AHBaseCharacterMesh = GetMesh();
	if (AHBaseCharacterMesh)
	{
		FVector& MeshRelativeLocation = AHBaseCharacterMesh->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = GetMesh()->GetRelativeLocation().Z + HalfHeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = BaseTranslationOffset.Z + HalfHeightAdjust;
	}

	K2_OnStartProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void AAHBaseCharacter::OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	RecalculateBaseEyeHeight();

	const AAHBaseCharacter* DefaultChar = GetDefault<AAHBaseCharacter>(GetClass());

	USkeletalMeshComponent* AHBaseCharacterMesh = GetMesh();
	if (AHBaseCharacterMesh && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = AHBaseCharacterMesh->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = GetMesh()->GetRelativeLocation().Z - HalfHeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = BaseTranslationOffset.Z - HalfHeightAdjust;
	}

	K2_OnEndProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void AAHBaseCharacter::OnSwimStart_Implementation()
{

}

void AAHBaseCharacter::OnSwimEnd_Implementation()
{

}

bool AAHBaseCharacter::CanProne()
{
	return !bIsProning && GetBaseCharacterMovementComponent() && GetBaseCharacterMovementComponent()->CanEverProne();
}

void AAHBaseCharacter::OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime)
{

}

void AAHBaseCharacter::OnDeath()
{
// 	GetCharacterMovement()->DisableMovement();
// 	float Duration = PlayAnimMontage(OnDeathAnimMontage);
// 	if (Duration == 0)
// 	{
// 		EnableRagdoll();
// 	}
	CharacterAttributeComponent->SetHealth(100);
}

void AAHBaseCharacter::OnOutOfStamina(bool IsOutOfStamina)
{
	GetBaseCharacterMovementComponent()->SetIsOutOfStamina(IsOutOfStamina);
}

void AAHBaseCharacter::OnOutOfOxygen(bool IsOutOfOxygen)
{
	if (IsOutOfOxygen && !GetWorld()->GetTimerManager().IsTimerActive(OutOfOxygenTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(OutOfOxygenTimer, this, &AAHBaseCharacter::OutOfOxygenDamage, CharacterAttributeComponent->GetOutOfOxygenDamageRate(), true);
	}
	else if (!IsOutOfOxygen)
	{
		GetWorld()->GetTimerManager().ClearTimer(OutOfOxygenTimer);
	}
}

void AAHBaseCharacter::OnStartAimingInternal()
{
	if (OnAimingStateChanged.IsBound())
	{
		OnAimingStateChanged.Broadcast(true);
	}
}

void AAHBaseCharacter::OnStopAimingInternal()
{
	if (OnAimingStateChanged.IsBound())
	{
		OnAimingStateChanged.Broadcast(false);
	}
}

void AAHBaseCharacter::TraceLineOfSight()
{
	if (!IsPlayerControlled())
	{
		return;
	}

	FVector ViewLocation;
	FRotator ViewRotation;

	APlayerController* PlayerController = GetController<APlayerController>();
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

	FVector ViewDirection = ViewRotation.Vector();
	FVector TraceEnd = ViewLocation + ViewDirection * LineTraceSightDistance;

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, ViewLocation, TraceEnd, ECC_Visibility);
	if (LineOfSightObject.GetObject() != HitResult.GetActor())
	{
		LineOfSightObject = HitResult.GetActor();

		FName ActionName;
		if (LineOfSightObject.GetInterface())
		{
			ActionName = LineOfSightObject->GetActionEventName();
		}
		else
		{
			ActionName = NAME_None;
		}
	}

}

void AAHBaseCharacter::OnRep_IsMantling(bool bWasMantling)
{
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (!bWasMantling && bIsMantling)
		{
			Mantle(true);
		}
	}
}

void AAHBaseCharacter::OnRep_IsOnLadder(bool bWasOnLadder)
{
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (bWasOnLadder != bIsOnLadder)
		{
			InteractWithLadder();
		}
	}
}

void AAHBaseCharacter::OnRep_IsOnZipline(bool bWasOnZipline)
{
	if (GetLocalRole() == ROLE_SimulatedProxy)
	{
		if (bWasOnZipline != bIsOnZipline)
		{
			InteractWithZipline();
		}
	}
}

void AAHBaseCharacter::StopJumping()
{
	Super::StopJumping();
}

void AAHBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, GetIKOffsetForASocket(RightFootSocketName), DeltaTime, IKInterpSpeed);
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, GetIKOffsetForASocket(LeftFootSocketName), DeltaTime, IKInterpSpeed);

	TryChangeSprintState(DeltaTime);
	TryChangeFastSwimState(DeltaTime);

	TraceLineOfSight();
}

void AAHBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AAIController* AIController = Cast<AAIController>(NewController);
	if (IsValid(AIController))
	{
		FGenericTeamId TeamID((uint8)Team);
		AIController->SetGenericTeamId(TeamID);
	}
}

void AAHBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAHBaseCharacter, bIsMantling);
	DOREPLIFETIME(AAHBaseCharacter, bIsOnLadder);
	DOREPLIFETIME(AAHBaseCharacter, bIsOnZipline);
	DOREPLIFETIME(AAHBaseCharacter, bIsOutOfStamina);
	DOREPLIFETIME(AAHBaseCharacter, bIsAiming);
	DOREPLIFETIME(AAHBaseCharacter, bIsMeleeAttacking);
	DOREPLIFETIME(AAHBaseCharacter, CurrentMeleeAttackType);
}

void AAHBaseCharacter::Mantle(bool bForce /*= false*/)
{
	if (!(CanMantle() || bForce))
	{
		return;
	}

	FLedgeDescription LedgeDescription;
	if (LedgeDetectorComponent->DetectLedge(LedgeDescription))
	{
		bIsMantling = true;

		FMantlingMovementParameters MantlingParametrs;

		MantlingParametrs.InitialLocation = GetActorLocation();
		MantlingParametrs.InitialRotation = GetActorRotation();
		MantlingParametrs.TargetLocation = LedgeDescription.Location;
		MantlingParametrs.TargetRotation = LedgeDescription.Rotation;
		MantlingParametrs.LedgeActor = LedgeDescription.LedgeActor;
		MantlingParametrs.TargetOffset = LedgeDescription.LedgeActor->GetActorLocation() - LedgeDescription.Location;

		float MantlingHeight = (MantlingParametrs.TargetLocation - MantlingParametrs.InitialLocation).Z;
		const FMantlingSettings& MantlingSettings = GetMantlingSettings(MantlingHeight);

		float MinRange;
		float MaxRange;
		MantlingSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);

		MantlingParametrs.Duration = MaxRange - MinRange;

		MantlingParametrs.MantlingCurve = MantlingSettings.MantlingCurve;

		FVector2D SourceRange(MantlingSettings.MinHeight, MantlingSettings.MaxHeight);
		FVector2D TargetRange(MantlingSettings.MinHeightStartTime, MantlingSettings.MaxHeightStartTime);
		MantlingParametrs.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);

		MantlingParametrs.InitialAimationLocation = MantlingParametrs.TargetLocation - MantlingSettings.AnimationCorrectionZ * FVector::UpVector + MantlingSettings.AnimationCorrectionXY * LedgeDescription.LedgeNormal;


		CharacterEquipmentComponent->EquipItemInSlot(EEquipmentSlots::None, true);
		if (GetLocalRole() > ROLE_SimulatedProxy)
		{
			GetBaseCharacterMovementComponent()->StartMantle(MantlingParametrs);
		}

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MantlingSettings.MantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingParametrs.StartTime);
		OnMantle(MantlingSettings, MantlingParametrs.StartTime);
	}
}

float AAHBaseCharacter::GetAimingMovementSpeed() const
{
	return CurrentAimingMovementSpeed;
}

FRotator AAHBaseCharacter::GetAimOffset()
{
	FVector AimDirectionWorld = GetBaseAimRotation().Vector();
	FVector AimDirectionLocal = GetTransform().InverseTransformVectorNoScale(AimDirectionWorld);
	FRotator Result = AimDirectionLocal.ToOrientationRotator();

	return Result;
}

void AAHBaseCharacter::OnStartAiming_Implementation()
{
	OnStartAimingInternal();
}

void AAHBaseCharacter::OnStopAiming_Implementation()
{
	OnStopAimingInternal();
}

void AAHBaseCharacter::Reload()
{
	if (IsValid(CharacterEquipmentComponent->GetCurrentRangeWeapon()))
	{
		CharacterEquipmentComponent->ReloadCurrentWeapon();
	}
}

void AAHBaseCharacter::NextItem()
{
	CharacterEquipmentComponent->EquipNextItem();
}

void AAHBaseCharacter::PreviousItem()
{
	CharacterEquipmentComponent->EquipPreviousItem();
}

void AAHBaseCharacter::EquipPrimaryItem()
{
	CharacterEquipmentComponent->EquipItemInSlot(EEquipmentSlots::PrimaryItemSlot);
}

void AAHBaseCharacter::PrimaryMeleeAttack()
{
	if (IsMeleeAttacking())
	{
		return;
	}

	if (GetBaseCharacterMovementComponent()->CanMeleeAttack())
	{
		AMeleeWeapon* CurrentMeleeWeapon = CharacterEquipmentComponent->GetCurrentMeleeWeapon();
		if (IsValid(CurrentMeleeWeapon))
		{
			CurrentMeleeWeapon->StartAttack(EMeleeAttackTypes::PrimaryAttack);
			CurrentMeleeWeapon->OnAttackEnded.AddUFunction(this, FName("OnMeleeAttackEnded"));
			if (IsLocallyControlled())
			{
				CurrentMeleeAttackType = EMeleeAttackTypes::PrimaryAttack;
				if (!HasAuthority())
				{
					Server_SetCurrentMeleeAttackType(EMeleeAttackTypes::PrimaryAttack);
					Server_StartMeleeAttack(EMeleeAttackTypes::PrimaryAttack); 
				}
			}
			bIsMeleeAttacking = true;
		}
	}
}

void AAHBaseCharacter::SecondaryMeleeAttack()
{
	if (IsMeleeAttacking())
	{
		return;
	}

	if (GetBaseCharacterMovementComponent()->CanMeleeAttack())
	{
		AMeleeWeapon* CurrentMeleeWeapon = CharacterEquipmentComponent->GetCurrentMeleeWeapon();
		if (IsValid(CurrentMeleeWeapon))
		{
			CurrentMeleeWeapon->StartAttack(EMeleeAttackTypes::SecondaryAttack);
			CurrentMeleeWeapon->OnAttackEnded.AddUFunction(this, FName("OnMeleeAttackEnded"));
			if (IsLocallyControlled())
			{
				CurrentMeleeAttackType = EMeleeAttackTypes::SecondaryAttack;
				if (!HasAuthority())
				{
					Server_SetCurrentMeleeAttackType(EMeleeAttackTypes::SecondaryAttack);
					Server_StartMeleeAttack(EMeleeAttackTypes::SecondaryAttack);
				}
			}
			bIsMeleeAttacking = true;
		}
	}
}

bool AAHBaseCharacter::IsMeleeAttacking()
{
	return bIsMeleeAttacking && IsLocallyControlled();
}

void AAHBaseCharacter::RegisterInteractiveActor(AInteractiveActor* IntaractiveActor)
{
	AvailableInteractiveActors.AddUnique(IntaractiveActor);
}

void AAHBaseCharacter::UnregisterInteractiveActor(AInteractiveActor* IntaractiveActor)
{
	AvailableInteractiveActors.RemoveSingleSwap(IntaractiveActor);
}

void AAHBaseCharacter::Falling()
{
	GetCharacterMovement()->bNotifyApex = true;
}

void AAHBaseCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	CurrentFallApex = GetActorLocation();
}

void AAHBaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	float FallHeight = (CurrentFallApex - GetActorLocation()).Z * 0.01f;
	if (IsValid(FallDamageCurve))
	{
		float DamageAmount = FallDamageCurve->GetFloatValue(FallHeight);
		TakeDamage(DamageAmount, FDamageEvent(), GetController(), Hit.GetActor());
	}
}

void AAHBaseCharacter::Interact()
{
	if (LineOfSightObject.GetInterface())
	{
		LineOfSightObject->Interact(this);
	}
}

FGenericTeamId AAHBaseCharacter::GetGenericTeamId() const
{
	return FGenericTeamId(uint8(Team));
}

bool AAHBaseCharacter::CanMantle()
{
	return GetBaseCharacterMovementComponent() && GetBaseCharacterMovementComponent()->CanAttemptMantle();
}

bool AAHBaseCharacter::CanJumpInternal_Implementation() const
{
	bool a = Super::CanJumpInternal_Implementation();
	bool aa = JumpIsAllowedInternal();

	bool bJumpIsAllowed = GetBaseCharacterMovementComponent()->CanAttemptJump();

	if (bJumpIsAllowed)
	{
		// Ensure JumpHoldTime and JumpCount are valid.
		if (!bWasJumping || GetJumpMaxHoldTime() <= 0.0f)
		{
			if (JumpCurrentCount == 0 && GetBaseCharacterMovementComponent()->IsFalling())
			{
				bJumpIsAllowed = JumpCurrentCount + 1 < JumpMaxCount;
			}
			else
			{
				bJumpIsAllowed = JumpCurrentCount < JumpMaxCount;
			}
		}
		else
		{
			// Only consider JumpKeyHoldTime as long as:
			// A) The jump limit hasn't been met OR
			// B) The jump limit has been met AND we were already jumping
			const bool bJumpKeyHeld = (bPressedJump && JumpKeyHoldTime < GetJumpMaxHoldTime());
			bJumpIsAllowed = bJumpKeyHeld &&
				((JumpCurrentCount < JumpMaxCount) || (bWasJumping && JumpCurrentCount == JumpMaxCount));
		}
	}

	bool c = !GetBaseCharacterMovementComponent()->IsMantling();
	bool d = !GetBaseCharacterMovementComponent()->IsProning();
	bool e = !GetBaseCharacterMovementComponent()->IsSwimming();
	return Super::CanJumpInternal_Implementation() && GetBaseCharacterMovementComponent() 
		&& !GetBaseCharacterMovementComponent()->IsMantling() 
		&& !GetBaseCharacterMovementComponent()->IsProning() 
		&& !GetBaseCharacterMovementComponent()->IsSwimming();
}

void AAHBaseCharacter::OnSprintStart_Implementation()
{

}

void AAHBaseCharacter::OnSprintEnd_Implementation()
{

}

void AAHBaseCharacter::OnFastSwimStart_Implementation()
{

}

void AAHBaseCharacter::OnFastSwimEnd_Implementation()
{

}

bool AAHBaseCharacter::CanSprint()
{
	return GetBaseCharacterMovementComponent()->CanEverSprint() 
		&& (GetBaseCharacterMovementComponent()->MovementMode != MOVE_Swimming) 
		&& !GetBaseCharacterMovementComponent()->IsOnLadder()
		&& !GetBaseCharacterMovementComponent()->IsOnZipline()
		&& !IsMeleeAttacking();
}

bool AAHBaseCharacter::CanFastSwim()
{
	return GetBaseCharacterMovementComponent()->MovementMode == MOVE_Swimming && !GetBaseCharacterMovementComponent()->IsOutOfStamina();

}

void AAHBaseCharacter::OnMeleeAttackEnded()
{
	AMeleeWeapon* CurrentMeleeWeapon = CharacterEquipmentComponent->GetCurrentMeleeWeapon();
	if (IsValid(CurrentMeleeWeapon))
	{
		CurrentMeleeWeapon->OnAttackEnded.RemoveAll(this);
	}

	bIsMeleeAttacking = false;
	CurrentMeleeAttackType = EMeleeAttackTypes::None;
	Server_SetCurrentMeleeAttackType(EMeleeAttackTypes::None);
	Server_EndMeleeAttack();
}

void AAHBaseCharacter::OnRep_bIsMeleeAttacking(bool bWasMeleeAttacking)
{
  	if (bIsMeleeAttacking)
  	{
		switch (CurrentMeleeAttackType)
		{
			case EMeleeAttackTypes::PrimaryAttack:
			{
				PrimaryMeleeAttack();
				break;
			}
			case EMeleeAttackTypes::SecondaryAttack:
			{
				SecondaryMeleeAttack();
				break;
			}
			default:
				break;
		}
	}
}

void AAHBaseCharacter::OnRep_bIsAiming(bool bWasAiming)
{

	if (bWasAiming && !bIsAiming)
	{
		StopAiming();
	}

	if (!bWasAiming && bIsAiming)
	{
		StartAiming();
	}

}

void AAHBaseCharacter::Server_StartAiming_Implementation()
{
	StartAiming();
}

void AAHBaseCharacter::Server_StopAiming_Implementation()
{
	StopAiming();
}

void AAHBaseCharacter::Server_StartMeleeAttack_Implementation(EMeleeAttackTypes MeleeAttackType)
{
	StartMeleeAttackByType(MeleeAttackType);
}

void AAHBaseCharacter::Server_EndMeleeAttack_Implementation()
{
	CurrentMeleeAttackType = EMeleeAttackTypes::None;
	bIsMeleeAttacking = false;
}

void AAHBaseCharacter::Server_SetCurrentMeleeAttackType_Implementation(EMeleeAttackTypes MeleeAttackType)
{
	CurrentMeleeAttackType = MeleeAttackType;
}

void AAHBaseCharacter::TryChangeSprintState(float DeltaTime)
{

	if ((bIsSprintRequested && !GetBaseCharacterMovementComponent()->IsSprinting()) || CanSprint())
	{
		GetBaseCharacterMovementComponent()->StartSprint();
		OnSprintStart();
	}

	if ((!bIsSprintRequested && GetBaseCharacterMovementComponent()->IsSprinting()) || !CanSprint() || GetBaseCharacterMovementComponent()->IsFalling())
	{
		GetBaseCharacterMovementComponent()->StopSprint();
		OnSprintEnd();
	}
}

void AAHBaseCharacter::TryChangeFastSwimState(float DeltaTime)
{
	if ((bIsFastSwimRequested && !GetBaseCharacterMovementComponent()->IsFastSwimming()) || CanFastSwim())
	{
		GetBaseCharacterMovementComponent()->StartFastSwim();
		OnFastSwimStart();
	}

	if ((!bIsFastSwimRequested && GetBaseCharacterMovementComponent()->IsFastSwimming()) || !CanFastSwim())
	{
		GetBaseCharacterMovementComponent()->StopFastSwim();
		OnFastSwimEnd();
	}
}

void AAHBaseCharacter::OutOfOxygenDamage()
{
	TakeDamage(CharacterAttributeComponent->GetOutOfOxygenDamageAmount(), FDamageEvent(), GetController(), this);
}

float AAHBaseCharacter::GetIKOffsetForASocket(const FName& SocketName)
{
	float Result = 0.0f;

	FVector SocketLocation = GetMesh()->GetSocketLocation(SocketName);
	FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);
	FVector TraceEnd = TraceStart - IKTraceDistance * FVector::UpVector;

	FHitResult HitResult;
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TraceEnd, TraceType, true, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true))
	{
		Result = -(TraceEnd.Z - HitResult.Location.Z) / GetActorScale3D().Z;
	}
	else if(UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceEnd, TraceEnd - IKTraceExtedDistance * FVector::UpVector, TraceType, true, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true))
	{
		Result = -(TraceEnd.Z - HitResult.Location.Z) / GetActorScale3D().Z;
	}

	return Result;
}

const FMantlingSettings& AAHBaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	return LedgeHeight > LowMantleMaxHeight ? HighMantleSettings : LowMantleSettings;
}

void AAHBaseCharacter::EnableRagdoll()
{
	GetMesh()->SetCollisionProfileName(CollisionProfileRagdoll);
	GetMesh()->SetSimulatePhysics(true);
}

void AAHBaseCharacter::StartMeleeAttackByType(EMeleeAttackTypes MeleeAttackType)
{
	switch (MeleeAttackType)
	{
	case EMeleeAttackTypes::PrimaryAttack:
	{
		PrimaryMeleeAttack();
		break;
	}
	case EMeleeAttackTypes::SecondaryAttack:
	{
		SecondaryMeleeAttack();
		break;
	}

	default:
		break;
	}
}

void AAHBaseCharacter::ClimbLadderUp(float Value)
{
	if (!FMath::IsNearlyZero(Value) && GetBaseCharacterMovementComponent()->IsOnLadder() && GetBaseCharacterMovementComponent()->GetCurrentLadder())
	{
		FVector LadderUpVector = GetBaseCharacterMovementComponent()->GetCurrentLadder()->GetActorUpVector();
		AddMovementInput(LadderUpVector, Value);
	}
}

void AAHBaseCharacter::InteractWithLadder()
{
	if (GetBaseCharacterMovementComponent()->IsOnLadder())
	{
		bIsOnLadder = false;
		CharacterEquipmentComponent->EquipItemInSlot(CharacterEquipmentComponent->GetPreviousEquipmentSlot(), true);
		GetBaseCharacterMovementComponent()->DetachFromLadder(EDetachFromLadderMethod::JumpOff);
	}
	else
	{
		const ALadder* AvailableLadder = GetAvailableLadder();
		if (IsValid(AvailableLadder))
		{
			if (AvailableLadder->GetIsOnTop()) 
			{
				PlayAnimMontage(AvailableLadder->GetAttachFromTopAnimMontage());
			}
			bIsOnLadder = true;
			CharacterEquipmentComponent->EquipItemInSlot(EEquipmentSlots::None, true);
			GetBaseCharacterMovementComponent()->AttachToLadder(AvailableLadder);
		}
	}
}

const class ALadder* AAHBaseCharacter::GetAvailableLadder() const
{
	const ALadder* Result = nullptr;

	for (const AInteractiveActor* InteractiveActor : AvailableInteractiveActors)
	{
		if (InteractiveActor->IsA<ALadder>())
		{
			Result = StaticCast<const ALadder*>(InteractiveActor);
			break;
		}
	}

	return Result;
}

void AAHBaseCharacter::InteractWithZipline()
{
	if (GetBaseCharacterMovementComponent()->IsOnZipline())
	{
		bIsOnZipline = false;
		CharacterEquipmentComponent->EquipItemInSlot(CharacterEquipmentComponent->GetPreviousEquipmentSlot(), true);
		GetBaseCharacterMovementComponent()->DetachFromZipline();
	}
	else
	{
		const AZipline* AvailableZipline = GetAvailableZipline();
		if (IsValid(AvailableZipline))
		{
			bIsOnZipline = true;
			CharacterEquipmentComponent->EquipItemInSlot(EEquipmentSlots::None, true);
			GetBaseCharacterMovementComponent()->AttachToZipline(AvailableZipline);

			FVector ZiplineDirection = AvailableZipline->GetZiplineDirection();
			ZiplineDirection.Normalize();
		}
	}
}

const class AZipline* AAHBaseCharacter::GetAvailableZipline() const
{
	const AZipline* Result = nullptr;

	for (const AInteractiveActor* InteractiveActor : AvailableInteractiveActors)
	{
		if (InteractiveActor->IsA<AZipline>())
		{
			Result = StaticCast<const AZipline*>(InteractiveActor);
			break;
		}
	}

	return Result;
}

void AAHBaseCharacter::StartFire()
{
	if (!CanFire())
	{
		return;
	}

	ARangeWeapon* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StartFire();
	}
}

bool AAHBaseCharacter::CanFire()
{
	if (CharacterEquipmentComponent->IsEquipping() && GetBaseCharacterMovementComponent()->CanFire())
	{
		return false;
	}

	return true;
}

void AAHBaseCharacter::StopFire()
{
	ARangeWeapon* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopFire(); 
	}
}

void AAHBaseCharacter::StartAiming()
{
	ARangeWeapon* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if (!IsValid(CurrentRangeWeapon))
	{
		return;
	}
	
	CurrentAimingMovementSpeed = CurrentRangeWeapon->GetAimMovementMaxSpeed();

	bIsAiming = true;

	CurrentRangeWeapon->StartAim();
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_StartAiming();

	}
	OnStartAiming();
}

void AAHBaseCharacter::StopAiming()
{
	if (!bIsAiming)
	{
		return;
	}

	ARangeWeapon* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if (!IsValid(CurrentRangeWeapon))
	{
		return;
	}

	
	bIsAiming = false;
	CurrentAimingMovementSpeed = 0;

	CurrentRangeWeapon->StopAim();
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_StopAiming();
	}
	OnStopAiming();
}

