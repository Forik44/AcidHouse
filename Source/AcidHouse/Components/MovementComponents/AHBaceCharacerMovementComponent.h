// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../LedgeDetectorComponent.h"
#include "AHBaceCharacerMovementComponent.generated.h"

struct FMantlingMovementParameters
{
	FVector InitialLocation = FVector::ZeroVector;
	FRotator InitialRotation = FRotator::ZeroRotator;

	FVector TargetLocation = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;

	FVector InitialAimationLocation = FVector::ZeroVector;

	float Duration = 1.0f;
	float StartTime = 0.0f;

	UCurveVector* MantlingCurve;

	TWeakObjectPtr<class AActor> LedgeActor;
	
	FVector TargetOffset = FVector::ZeroVector;
};

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	CMOVE_None = 0 UMETA(DisplayName = "None"),
	CMOVE_Mantling UMETA(DisplayName = "Mantling"),
	CMOVE_Max UMETA(Hidden)
};
/**
 * 
 */
UCLASS()
class ACIDHOUSE_API UAHBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	bool bWantsToProne = false;

	virtual void BeginPlay() override;

	FORCEINLINE bool IsSprinting() const { return bIsSprinting; }
	FORCEINLINE bool IsProning() const;
	FORCEINLINE bool CanEverProne() { return bCanEverProne; }
	FORCEINLINE bool IsFastSwimming() const { return bIsFastSwimming; }

	FORCEINLINE float GetSwimmingCapsuleHalfHeight() const { return SwimmingCapsuleHalfHeight; }
	FORCEINLINE float GetSwimmingCapsuleRadius() const { return SwimmingCapsuleRadius; }

	virtual float GetMaxSpeed() const override;

	void StartSprint();
	void StopSprint();

	bool CanEverSprint();

	void StartFastSwim();
	void StopFastSwim();

	virtual bool CanCrouchInCurrentState() const override;

	virtual bool CanAttemptJump() const override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	FORCEINLINE bool IsOutOfStamina() const { return bIsOutOfStamina; }
	void SetIsOutOfStamina(bool bIsOutOfStamina_In);

	virtual bool CanProneInCurrentState() const;
	virtual void Prone();
	virtual void UnProne();

	void StartMantle(const FMantlingMovementParameters& MantlingParameters);
	void EndMantle();
	bool IsMantling() const;
	virtual bool CanAttemptMantle() const;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SprintSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float OutOfStaminaSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: prone")
	bool bCanEverProne = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: prone", meta = (ClampMin = 0, UIMin = 0))
	float ProneCapsuleRadus = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: prone", meta = (ClampMin = 0, UIMin = 0))
	float ProneCapsuleHalfHeight = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: prone", meta = (ClampMin = 0, UIMin = 0))
	float MaxProneSpeed = 100.0f;

	UPROPERTY(Category="Character Movement (General Settings)", VisibleInstanceOnly, BlueprintReadWrite, AdvancedDisplay)
	uint8 bProneMaintainsBaseLocation:1;

	UPROPERTY(Category="Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float SwimmingCapsuleRadius = 60.0f;

	UPROPERTY(Category="Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float SwimmingCapsuleHalfHeight = 60.0f;

	UPROPERTY(Category="Character Movement: Swimming", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0"))
	float FastSwimmingSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Swimming")
	bool bCanEverSwim = true;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviusCustomMode) override;

	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

private:
	bool bIsSprinting = false;
	bool bIsOutOfStamina = false;
	bool bIsFastSwimming = false;

	class AAHBaseCharacter* CachedAHBaseCharacter;

	FMantlingMovementParameters CurrentMantlingParametrs;

	FTimerHandle MantlingTimer;
};
