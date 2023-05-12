// Fill out your copyright notice in the Description page of Project Settings.


#include "AHBaseCharacterAnimInstance.h"
#include "Characters/AHBaseCharacter.h"
#include "Components/MovementComponents/AHBaceCharacerMovementComponent.h" 

void UAHBaseCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AAHBaseCharacter>(), TEXT("UAHBaseCharacterAnimInstance can be used only with AAHBaseCharacter"));
	CachedBaseCharacter = StaticCast<AAHBaseCharacter*>(TryGetPawnOwner());
}

void UAHBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(!CachedBaseCharacter.IsValid())
	{
		return;
	}

	UAHBaseCharacterMovementComponent* CharacterMovement = CachedBaseCharacter->GetBaseCharacterMovementComponent();
	Speed = CharacterMovement->Velocity.Size();
	bIsFalling = CharacterMovement->IsFalling();
	bIsCrouching = CharacterMovement->IsCrouching();
	bIsSprinting = CharacterMovement->IsSprinting();
	bIsOutOfStamina = CharacterMovement->IsOutOfStamina();
	bIsProning = CharacterMovement->IsProning();
	bIsSwimming = CharacterMovement->IsSwimming();
	bIsOnLadder = CharacterMovement->IsOnLadder();
	if (bIsOnLadder)
	{
		LadderSpeedRation = CharacterMovement->GetLadderSpeedRation();
	}
	bIsOnZipline = CharacterMovement->IsOnZipline();

	bIsStrafing = !CharacterMovement->bOrientRotationToMovement;
	Direction = CalculateDirection(CharacterMovement->Velocity, CachedBaseCharacter->GetActorRotation());

	RightFootEffectorLocation = FVector(0.0f, 0.0f, CachedBaseCharacter->GetIKRightFootOffset());
	LeftFootEffectorLocation = FVector(0.0f, 0.0f, CachedBaseCharacter->GetIKLeftFootOffset());
}
