// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AHBaseCharacter.generated.h"

class UAHBaseCharacterMovementComponent;
UCLASS(Abstract, NotBlueprintable)
class ACIDHOUSE_API AAHBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAHBaseCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	bool bIsProning = false;

	virtual void MoveForward(float Value) {};
	virtual void MoveRight(float Value) {};
	virtual void Turn(float Value) {};
	virtual void LookUp(float Value) {};

	virtual void SwimForward(float Value) {};
	virtual void SwimRight(float Value) {};
	virtual void SwimUp(float Value) {};

	virtual void TryJump();

	virtual void ChangeCrouchState();
	virtual void ChangeProneState();

	virtual void Prone();
	virtual void UnProne();

	virtual void StartSprint();
	virtual void StopSprint();

	virtual void StartFastSwim();
	virtual void StopFastSwim();

	virtual void OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartProne", ScriptName="OnStartProne"))
	void K2_OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	virtual void OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartProne", ScriptName="OnStartProne"))
	void K2_OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSwimStart();
	virtual void OnSwimStart_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSwimEnd();
	virtual void OnSwimEnd_Implementation();

	virtual bool CanProne();

	virtual void Tick(float DeltaTime) override;

	virtual void Mantle();

	FORCEINLINE UAHBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() { return AHBaseCharacterMovementComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKRightFootOffset() const { return IKRightFootOffset; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKLeftFootOffset() const { return IKLeftFootOffset; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetCurrentStamina() const { return CurrentStamina; }

	bool IsLedgeDetectionDebugDrawEnabled() const { return bIsLedgeDetectionDebugDrawEnabled; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement")
	float SprintSpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement", meta = (ClampMin = 0, UIMin = 0))
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement", meta = (ClampMin = 0, UIMin = 0))
	float StaminaRestoreVelocity = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement", meta = (ClampMin = 0, UIMin = 0))
	float SprintStaminaConsumptionVelocity = 12.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK settings")
	FName RightFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK settings")
	FName LeftFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK settings", meta = (ClampMin = 0, UIMin = 0))
	float IKTraceExtedDistance = 30.0f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Character | IK settings", meta = (ClampMin = 0, UIMin = 0))
	float IKInterpSpeed = 20.0f; 

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSprintStart();
	virtual void OnSprintStart_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSprintEnd();
	virtual void OnSprintEnd_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnFastSwimStart();
	virtual void OnFastSwimStart_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnFastSwimEnd();
	virtual void OnFastSwimEnd_Implementation();

	virtual bool CanSprint();
	virtual bool CanFastSwim();
	UAHBaseCharacterMovementComponent* AHBaseCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	class ULedgeDetectorComponent* LedgeDetectorComponent;

private:
	bool bIsSprintRequested = false;
	bool bIsFastSwimRequested = false;

	float CurrentStamina = 0.0f;

	float IKRightFootOffset = 0.0f;
	float IKLeftFootOffset = 0.0f;
	
	float IKTraceDistance = 0.0f;

	bool bIsLedgeDetectionDebugDrawEnabled = false;

	UFUNCTION(exec)
	void ToggleLedgeDetectionDebugDraw() { bIsLedgeDetectionDebugDrawEnabled = !bIsLedgeDetectionDebugDrawEnabled; }

	void TryChangeSprintState(float DeltaTime);

	void TryChangeFastSwimState(float DeltaTime);

	float GetIKOffsetForASocket(const FName& SocketName);
};
