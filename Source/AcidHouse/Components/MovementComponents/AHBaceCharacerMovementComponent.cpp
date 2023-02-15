// Fill out your copyright notice in the Description page of Project Settings.


#include "AHBaceCharacerMovementComponent.h"

float UAHBaseCharacterMovementComponent::GetMaxSpeed() const
{
	float Result = Super::GetMaxSpeed();
	if (bIsSprinting)
	{
		Result = SprintSpeed;
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

bool UAHBaseCharacterMovementComponent::CanEverSprint()
{
	return !FMath::IsNearlyZero(GetOwner()->GetVelocity().Size(), 1e-6f);
}
