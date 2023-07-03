#include "AHBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/MovementComponents/AHBaceCharacerMovementComponent.h"
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
	if (CanJump())
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
	GetCharacterMovement()->DisableMovement();
	float Duration = PlayAnimMontage(OnDeathAnimMontage);
	if (Duration == 0)
	{
		EnableRagdoll();
	}

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

void AAHBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, GetIKOffsetForASocket(RightFootSocketName), DeltaTime, IKInterpSpeed);
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, GetIKOffsetForASocket(LeftFootSocketName), DeltaTime, IKInterpSpeed);

	TryChangeSprintState(DeltaTime);
	TryChangeFastSwimState(DeltaTime);
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

		GetBaseCharacterMovementComponent()->StartMantle(MantlingParametrs);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MantlingSettings.MantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingParametrs.StartTime);
		OnMantle(MantlingSettings, MantlingParametrs.StartTime);
	}
}

float AAHBaseCharacter::GetAimingMovementSpeed() const
{
	return CurrentAimingMovementSpeed;
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
		TakeDamage(DamageAmount, FDamageEvent(), GetController(), Hit.Actor.Get());
	}
}

bool AAHBaseCharacter::CanMantle()
{
	return GetBaseCharacterMovementComponent() && GetBaseCharacterMovementComponent()->CanAttemptMantle();
}

bool AAHBaseCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation() && GetBaseCharacterMovementComponent() 
		&& !GetBaseCharacterMovementComponent()->IsMantling() && !GetBaseCharacterMovementComponent()->IsProning() 
		&& !GetBaseCharacterMovementComponent()->IsFalling() && !GetBaseCharacterMovementComponent()->IsSwimming();
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
	return GetBaseCharacterMovementComponent()->CanEverSprint() && (GetBaseCharacterMovementComponent()->MovementMode != MOVE_Swimming) && !GetBaseCharacterMovementComponent()->IsOnLadder();
}

bool AAHBaseCharacter::CanFastSwim()
{
	return GetBaseCharacterMovementComponent()->MovementMode == MOVE_Swimming && !GetBaseCharacterMovementComponent()->IsOutOfStamina();

}

void AAHBaseCharacter::TryChangeSprintState(float DeltaTime)
{

	if ((bIsSprintRequested && !GetBaseCharacterMovementComponent()->IsSprinting()) || CanSprint())
	{
		GetBaseCharacterMovementComponent()->StartSprint();
		OnSprintStart();
	}

	if ((!bIsSprintRequested && GetBaseCharacterMovementComponent()->IsSprinting()) || !CanSprint())
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

void AAHBaseCharacter::ClimbLadderUp(float Value)
{
	if (!FMath::IsNearlyZero(Value) && GetBaseCharacterMovementComponent()->IsOnLadder())
	{
		FVector LadderUpVector = GetBaseCharacterMovementComponent()->GetCurrentLadder()->GetActorUpVector();
		AddMovementInput(LadderUpVector, Value);
	}
}

void AAHBaseCharacter::InteractWithLadder()
{
	if (GetBaseCharacterMovementComponent()->IsOnLadder())
	{
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

void AAHBaseCharacter::InteractWithZipline() const
{
	if (GetBaseCharacterMovementComponent()->IsOnZipline())
	{
		GetBaseCharacterMovementComponent()->DetachFromZipline();
	}
	else
	{
		const AZipline* AvailableZipline = GetAvailableZipline();
		if (IsValid(AvailableZipline))
		{
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
	if (CharacterEquipmentComponent->IsEquipping())
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

	OnStartAiming();
}

void AAHBaseCharacter::StopAiming()
{
	if (!bIsAiming)
	{
		return;
	}

	ARangeWeapon* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopAim();
	}

	bIsAiming = false;
	CurrentAimingMovementSpeed = 0;
	OnStopAiming();
}

