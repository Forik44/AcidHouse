// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerAnimInstance.h"
#include "../FPPlayerCharacter.h"

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
	APlayerController* Controller = ChachedFirstPersonCharacterOwner->GetController<APlayerController>();
	if (IsValid(Controller))
	{
		PlayerCameraPitchAngle = Controller->GetControlRotation().Pitch;
	}

}
