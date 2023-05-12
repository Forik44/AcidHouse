// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerAnimInstance.h"
#include "Characters/FPPlayerCharacter.h"
#include "Characters/Controllers/AHBasePlayerController.h"

void UFPPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AFPPlayerCharacter>(), TEXT("UFPPlayerAnimInstance::NativeBeginPlay() only can use"));

	ChachedFirstPersonCharacterOwner = StaticCast<AFPPlayerCharacter*>(TryGetPawnOwner());

}

void UFPPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!ChachedFirstPersonCharacterOwner.IsValid())
	{
		return;
	}
	
	PlayerCameraPitchAngle = CalculateCameraPitchAngle();
	

}

float UFPPlayerAnimInstance::CalculateCameraPitchAngle() const
{
	float Result = 0.0f;

	AAHBasePlayerController* Controller = ChachedFirstPersonCharacterOwner->GetController<AAHBasePlayerController>();
	if (IsValid(Controller) && !Controller->GetIgnoreCameraPitch())
	{
		
		Result = Controller->GetControlRotation().Pitch;
	}

	return Result;

}
