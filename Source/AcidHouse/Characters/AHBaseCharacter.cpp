// Fill out your copyright notice in the Description page of Project Settings.


#include "AHBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Components/MovementComponents/AHBaceCharacerMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"


AAHBaseCharacter::AAHBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: 
	Super(ObjectInitializer.SetDefaultSubobjectClass<UAHBaseCharacterMovementComponent> (ACharacter::CharacterMovementComponentName))
{
	AHBaseCharacterMovementComponent = StaticCast<UAHBaseCharacterMovementComponent*>(GetCharacterMovement());

	IKTraceDistance = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

void AAHBaseCharacter::ChangeCrouchState()
{
	if (AHBaseCharacterMovementComponent->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		Crouch();
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

void AAHBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, GetIKOffsetForASocket(RightFootSocketName), DeltaTime, IKInterpSpeed);
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, GetIKOffsetForASocket(LeftFootSocketName), DeltaTime, IKInterpSpeed);

	TryChangeSprintState();
}

void AAHBaseCharacter::OnSprintStart_Implementation()
{

}

void AAHBaseCharacter::OnSprintEnd_Implementation()
{

}

bool AAHBaseCharacter::CanSprint()
{
	return AHBaseCharacterMovementComponent->CanEverSprint();
}

void AAHBaseCharacter::TryChangeSprintState()
{
	if (bIsSprintRequested && !AHBaseCharacterMovementComponent->IsSprinting() && CanSprint())
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

float AAHBaseCharacter::GetIKOffsetForASocket(const FName& SocketName)
{
	float Result = 0.0f;

	FVector SocketLocation = GetMesh()->GetSocketLocation(SocketName);
	FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);
	FVector TraceEnd = TraceStart - IKTraceDistance * FVector::UpVector;

	FHitResult HitResult;
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TraceEnd, TraceType, true, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult, true))
	{
		Result = -(TraceEnd.Z - HitResult.Location.Z) / GetActorScale3D().Z;
	}
	else if(UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceEnd, TraceEnd - IKTraceExtedDistance * FVector::UpVector, TraceType, true, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult, true))
	{
		Result = -(TraceEnd.Z - HitResult.Location.Z) / GetActorScale3D().Z;
	}

	return Result;
}

