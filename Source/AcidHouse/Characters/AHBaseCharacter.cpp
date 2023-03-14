// Fill out your copyright notice in the Description page of Project Settings.


#include "AHBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Components/MovementComponents/AHBaceCharacerMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "../Components/LedgeDetectorComponent.h"
#include "Curves/CurveVector.h"


AAHBaseCharacter::AAHBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: 
	Super(ObjectInitializer.SetDefaultSubobjectClass<UAHBaseCharacterMovementComponent> (ACharacter::CharacterMovementComponentName))
{
	AHBaseCharacterMovementComponent = StaticCast<UAHBaseCharacterMovementComponent*>(GetCharacterMovement());
	CurrentStamina = MaxStamina;

	IKTraceDistance = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("LedgeDetector"));
}

void AAHBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

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

void AAHBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, GetIKOffsetForASocket(RightFootSocketName), DeltaTime, IKInterpSpeed);
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, GetIKOffsetForASocket(LeftFootSocketName), DeltaTime, IKInterpSpeed);

	TryChangeSprintState(DeltaTime);
	TryChangeFastSwimState(DeltaTime);

	if (!GetBaseCharacterMovementComponent()->IsSprinting() && !GetBaseCharacterMovementComponent()->IsFastSwimming())
	{
		CurrentStamina += StaminaRestoreVelocity * DeltaTime;
		CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);
		if (CurrentStamina == MaxStamina)
		{
			GetBaseCharacterMovementComponent()->SetIsOutOfStamina(false);
		}
	}
}

void AAHBaseCharacter::Mantle()
{
	FLedgeDescription LedgeDescription;
	if (LedgeDetectorComponent->DetectLedge(LedgeDescription) && CanMantle())
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
	return GetBaseCharacterMovementComponent()->CanEverSprint() && (GetBaseCharacterMovementComponent()->MovementMode != MOVE_Swimming);
}

bool AAHBaseCharacter::CanFastSwim()
{
	return GetBaseCharacterMovementComponent()->MovementMode == MOVE_Swimming && !GetBaseCharacterMovementComponent()->IsOutOfStamina();

}

void AAHBaseCharacter::TryChangeSprintState(float DeltaTime)
{
	if (GetBaseCharacterMovementComponent()->IsSprinting())
	{
		CurrentStamina -= SprintStaminaConsumptionVelocity * DeltaTime;
		CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);
		if (FMath::IsNearlyZero(CurrentStamina, 1e-6f))
		{
			GetBaseCharacterMovementComponent()->SetIsOutOfStamina(true);
		}
	}

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
	if (GetBaseCharacterMovementComponent()->IsFastSwimming())
	{
		CurrentStamina -= SprintStaminaConsumptionVelocity * DeltaTime;
		CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);
		if (FMath::IsNearlyZero(CurrentStamina, 1e-6f))
		{
			GetBaseCharacterMovementComponent()->SetIsOutOfStamina(true);
		}
	}

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

