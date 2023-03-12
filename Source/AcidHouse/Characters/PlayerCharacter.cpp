// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "../Components/MovementComponents/AHBaceCharacerMovementComponent.h"
#include "Curves/CurveFloat.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: 
	Super(ObjectInitializer)
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	GetBaseCharacterMovementComponent()->bOrientRotationToMovement = 1;
	GetBaseCharacterMovementComponent()->NavAgentProps.bCanCrouch = 1;
	GetBaseCharacterMovementComponent()->CrouchedHalfHeight = 60;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	DefaultSpringArmDistance = SpringArmComponent->TargetArmLength;
}

void APlayerCharacter::MoveForward(float Value)
{
	if ((GetBaseCharacterMovementComponent()->IsMovingOnGround() || GetBaseCharacterMovementComponent()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if ((GetBaseCharacterMovementComponent()->IsMovingOnGround() || GetBaseCharacterMovementComponent()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void APlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void APlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void APlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset -= FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void APlayerCharacter::OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void APlayerCharacter::OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset -= FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void APlayerCharacter::SwimForward(float Value)
{
	if (GetBaseCharacterMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator PitchYawRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = PitchYawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::SwimRight(float Value)
{
	if (GetBaseCharacterMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::SwimUp(float Value)
{
	if (GetBaseCharacterMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		AddMovementInput(FVector::UpVector, Value);
	}
}

void APlayerCharacter::OnFastSwimStart_Implementation()
{
	bIsFastSwimStarted = true;
}

void APlayerCharacter::OnFastSwimEnd_Implementation()
{
	bIsFastSwimStarted = false;
}

void APlayerCharacter::OnSwimStart_Implementation()
{
	/*Super::OnSwimStart_Implementation();
	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, AHBaseCharacterMovementComponent->GetSwimmingCapsuleHalfHeight());*/
}

void APlayerCharacter::OnSwimEnd_Implementation()
{
	Super::OnSwimStart_Implementation();
	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, AHBaseCharacterMovementComponent->GetSwimmingCapsuleHalfHeight()/2);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ((bIsFastSwimStarted || bIsSprintStarted) && CurrentSpringArmTime < 0.5)
	{
		AddCurrentSpringArmTime(DeltaTime);
		Alpha = SpringArmChangingFromTime(CurrentSpringArmTime);
		SpringArmComponent->TargetArmLength = FMath::Lerp(DefaultSpringArmDistance, MaxSprintSpringArmDistance, Alpha);
	}
	else if (!bIsFastSwimStarted && !bIsSprintStarted && CurrentSpringArmTime > 0)
	{
		DeleteCurrentSpringArmTime(DeltaTime);
		Alpha = SpringArmChangingFromTime(CurrentSpringArmTime);
		SpringArmComponent->TargetArmLength = FMath::Lerp(DefaultSpringArmDistance, MaxSprintSpringArmDistance, Alpha);
	}
}

void APlayerCharacter::OnSprintEnd_Implementation()
{
	bIsSprintStarted = false;
}

void APlayerCharacter::OnSprintStart_Implementation()
{
	bIsSprintStarted = true;
}

float APlayerCharacter::SpringArmChangingFromTime(float Time)
{
	float Result = 0.0f;
	if (IsValid(SpringArmChangingCurve))
	{
		Result = SpringArmChangingCurve->GetFloatValue(Time);
	}
	return Result;
}

void APlayerCharacter::AddCurrentSpringArmTime(float DeltaTime)
{
	CurrentSpringArmTime += DeltaTime;
	CurrentSpringArmTime = FMath::Clamp(CurrentSpringArmTime, 0.0f, 0.5f);
}

void APlayerCharacter::DeleteCurrentSpringArmTime(float DeltaTime)
{
	CurrentSpringArmTime -= DeltaTime;
	CurrentSpringArmTime = FMath::Clamp(CurrentSpringArmTime, 0.0f, 0.5f);

}

void APlayerCharacter::OnJumped_Implementation()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

bool APlayerCharacter::CanJumpInternal_Implementation() const
{
	return !GetBaseCharacterMovementComponent()->IsMantling() || bIsCrouched || Super::CanJumpInternal_Implementation();
}
