// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AHBaseCharacter.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ACIDHOUSE_API APlayerCharacter : public AAHBaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void Turn(float Value) override;
	virtual void LookUp(float Value) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void SwimForward(float Value) override;
	virtual void SwimRight(float Value) override;
	virtual void SwimUp(float Value) override;

	virtual void OnJumped_Implementation() override;

	virtual void OnSprintStart_Implementation() override;
	virtual void OnSprintEnd_Implementation() override;

	virtual void OnSwimStart_Implementation() override;
	virtual void OnSwimEnd_Implementation() override;

	virtual void OnFastSwimStart_Implementation() override;
	virtual void OnFastSwimEnd_Implementation() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Camera")
	class UCurveFloat* SpringArmChangingCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Camera")
	class UCurveFloat* AimingFOVChangingCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Camera", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxSprintSpringArmDistance = 380;

	UFUNCTION(BlueprintCallable)
	float GetCurveValue(UCurveFloat* Curve, float Time);

	virtual void OnStartAimingInternal() override;
	virtual void OnStopAimingInternal() override;

private:
	bool bIsSprintStarted;
	bool bIsFastSwimStarted;
	bool bIsAimingStarted;

	float CurrentSpringArmTime = 0.0f;
	float DefaultSpringArmDistance;
	float AlphaSpringArm;


	float CurrentFOVTime = 0.0f;
	float AlphaFOV = 0.0f;
	float AimingFOV;
	float DefaultFOV;

	class APlayerController* PlayerController;
	class APlayerCameraManager* PlayerCameraManager;
	class ARangeWeapon* CurrentRangeWeapon;

	void AddCurrentCurveTime(float DeltaTime, float& TypeCurrentTime, float MaxTime);
	void SubtractCurrentCurveTime(float DeltaTime, float& TypeCurrentTime, float MaxTime);
	
};
