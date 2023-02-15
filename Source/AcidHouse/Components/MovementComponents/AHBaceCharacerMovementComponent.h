// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AHBaceCharacerMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class ACIDHOUSE_API UAHBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	FORCEINLINE bool IsSprinting() { return bIsSprinting; }
	virtual float GetMaxSpeed() const override;

	void StartSprint();
	void StopSprint();

	bool CanEverSprint();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SprintSpeed = 1200.0f;

private:
	bool bIsSprinting;
};
