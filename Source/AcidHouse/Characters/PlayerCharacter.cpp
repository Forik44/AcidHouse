// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/MovementComponents/AHBaseCharacterMovementComponent.h"
#include "Curves/CurveFloat.h"
#include "Actors/Equipment/Weapon/RangeWeapon.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Controllers/AHBasePlayerController.h"

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
	GetBaseCharacterMovementComponent()->SetCrouchedHalfHeight(60);

	Team = ETeams::Player;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	DefaultSpringArmDistance = SpringArmComponent->TargetArmLength;

	PlayerController = GetController<AAHBasePlayerController>();
	if (!IsValid(PlayerController))
	{
		return;
	}

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
	if (IsValid(CurrentRangeWeapon))
	{
		AddControllerYawInput(Value * CurrentRangeWeapon->GetAimTurnModifier());
	}
	else
	{
		AddControllerYawInput(Value);
	}
}

void APlayerCharacter::LookUp(float Value)
{
	if (IsValid(CurrentRangeWeapon))
	{
		AddControllerPitchInput(Value * CurrentRangeWeapon->GetAimLookUpModifier());
	}
	else
	{
		AddControllerPitchInput(Value);
	}
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
		AddCurrentCurveTime(DeltaTime, CurrentSpringArmTime, 0.5f);
		AlphaSpringArm = GetCurveValue(SpringArmChangingCurve, CurrentSpringArmTime);
		SpringArmComponent->TargetArmLength = FMath::Lerp(DefaultSpringArmDistance, MaxSprintSpringArmDistance, AlphaSpringArm);
	}
	else if (!bIsFastSwimStarted && !bIsSprintStarted && CurrentSpringArmTime > 0)
	{
		SubtractCurrentCurveTime(DeltaTime, CurrentSpringArmTime, 0.5f);
		AlphaSpringArm = GetCurveValue(SpringArmChangingCurve,CurrentSpringArmTime);
		SpringArmComponent->TargetArmLength = FMath::Lerp(DefaultSpringArmDistance, MaxSprintSpringArmDistance, AlphaSpringArm);
	}
	
	if (bIsAimingStarted && CurrentFOVTime < 0.3 && IsValid(AimingFOVChangingCurve))
	{
		AddCurrentCurveTime(DeltaTime, CurrentFOVTime, 0.3f);
		AlphaFOV = GetCurveValue(AimingFOVChangingCurve, CurrentFOVTime);
		if (IsValid(PlayerCameraManager))
		{
			PlayerCameraManager->SetFOV(FMath::Lerp(DefaultFOV, AimingFOV, AlphaFOV));
		}
		
	}
	else if (!bIsAimingStarted && CurrentFOVTime > 0 && IsValid(AimingFOVChangingCurve))
	{
		SubtractCurrentCurveTime(DeltaTime, CurrentFOVTime, 0.3f);
		AlphaFOV = GetCurveValue(AimingFOVChangingCurve, CurrentFOVTime);
		if (IsValid(PlayerCameraManager))
		{
			PlayerCameraManager->SetFOV(FMath::Lerp(DefaultFOV, AimingFOV, AlphaFOV));
		}
	}
	else if (bIsAimingStarted && !IsValid(AimingFOVChangingCurve))
	{
		PlayerCameraManager->SetFOV(AimingFOV);
	}
	else if (!bIsAimingStarted && !IsValid(AimingFOVChangingCurve))
	{
		PlayerCameraManager->SetFOV(DefaultFOV);
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

float APlayerCharacter::GetCurveValue(UCurveFloat* Curve, float Time)
{
	float Result = 0.0f;
	if (IsValid(Curve))
	{
		Result = Curve->GetFloatValue(Time);
	}
	return Result;
}

void APlayerCharacter::OnStartAimingInternal()
{
	Super::OnStartAimingInternal();

	if (!IsValid(PlayerController))
	{
		return;
	}

	PlayerCameraManager = PlayerController->PlayerCameraManager;
	if (IsValid(PlayerCameraManager))
	{
		CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
		AimingFOV = CurrentRangeWeapon->GetAimFOV();
		DefaultFOV = PlayerCameraManager->DefaultFOV;
	}
	bIsAimingStarted = true;
}

void APlayerCharacter::OnStopAimingInternal()
{
	Super::OnStopAimingInternal();
	if (!IsValid(PlayerController))
	{
		return;
	}

	bIsAimingStarted = false;
}

void APlayerCharacter::AddCurrentCurveTime(float DeltaTime, float& TypeCurrentTime, float MaxTime)
{
	TypeCurrentTime += DeltaTime;
	TypeCurrentTime = FMath::Clamp(TypeCurrentTime, 0.0f, MaxTime);
}

void APlayerCharacter::SubtractCurrentCurveTime(float DeltaTime, float& TypeCurrentTime, float MaxTime)
{
	TypeCurrentTime -= DeltaTime;
	TypeCurrentTime = FMath::Clamp(TypeCurrentTime, 0.0f, MaxTime);
}

void APlayerCharacter::OnJumped_Implementation()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

