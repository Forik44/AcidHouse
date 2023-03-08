// Fill out your copyright notice in the Description page of Project Settings.


#include "AHBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Components/MovementComponents/AHBaceCharacerMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SkeletalMeshComponent.h"


AAHBaseCharacter::AAHBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: 
	Super(ObjectInitializer.SetDefaultSubobjectClass<UAHBaseCharacterMovementComponent> (ACharacter::CharacterMovementComponentName))
{
	AHBaseCharacterMovementComponent = StaticCast<UAHBaseCharacterMovementComponent*>(GetCharacterMovement());

	IKTraceDistance = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

void AAHBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentStamina = MaxStamina;
}

void AAHBaseCharacter::TryJump()
{
	if (!AHBaseCharacterMovementComponent->IsProning() && !AHBaseCharacterMovementComponent->IsCrouching())
	{
		Jump();
		return;
	} 
	
	if(AHBaseCharacterMovementComponent->IsProning())
	{
		UnProne();
	}
	
	if (AHBaseCharacterMovementComponent->IsCrouching())
	{
		UnCrouch();
	}
}

void AAHBaseCharacter::ChangeCrouchState()
{
	if (AHBaseCharacterMovementComponent->IsCrouching() && !AHBaseCharacterMovementComponent->CanProneInCurrentState())
	{
		UnCrouch();
	}
	else if (!AHBaseCharacterMovementComponent->IsCrouching())
	{
		Crouch();
	}
}

void AAHBaseCharacter::ChangeProneState()
{
	if (AHBaseCharacterMovementComponent->IsProning())
	{
		UnProne();
	}
	else if (AHBaseCharacterMovementComponent->IsCrouching())
	{
		Prone();
	}
}

void AAHBaseCharacter::Prone()
{
	if (AHBaseCharacterMovementComponent)
	{
		if (CanProne())
		{
			AHBaseCharacterMovementComponent->bWantsToProne = true;
		}
	}
}

void AAHBaseCharacter::UnProne()
{
	if (AHBaseCharacterMovementComponent)
	{
		AHBaseCharacterMovementComponent->bWantsToProne = false;
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
	return !bIsProning && AHBaseCharacterMovementComponent && AHBaseCharacterMovementComponent->CanEverProne();
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
	return AHBaseCharacterMovementComponent->CanEverSprint() && (GetBaseCharacterMovementComponent()->MovementMode != MOVE_Swimming);
}

bool AAHBaseCharacter::CanFastSwim()
{
	return GetBaseCharacterMovementComponent()->MovementMode == MOVE_Swimming && !çGetBaseCharacterMovementComponent()->IsOutOfStamina();

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

	if ((bIsSprintRequested && !AHBaseCharacterMovementComponent->IsSprinting()) || CanSprint())
	{
		AHBaseCharacterMovementComponent->StartSprint();
		OnSprintStart();
	}

	if ((!bIsSprintRequested && AHBaseCharacterMovementComponent->IsSprinting()) || !CanSprint())
	{
		AHBaseCharacterMovementComponent->StopSprint();
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

	if ((bIsFastSwimRequested && !AHBaseCharacterMovementComponent->IsFastSwimming()) || CanFastSwim())
	{
		AHBaseCharacterMovementComponent->StartFastSwim();
		OnFastSwimStart();
	}

	if ((!bIsFastSwimRequested && AHBaseCharacterMovementComponent->IsFastSwimming()) || !CanFastSwim())
	{
		AHBaseCharacterMovementComponent->StopFastSwim();
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

