// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AHBaseCharacterAnimInstance.h"
#include "FPPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ACIDHOUSE_API UFPPlayerAnimInstance : public UAHBaseCharacterAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Animations | FirstPerson")
	float PlayerCameraPitchAngle = 0.0f;

	TWeakObjectPtr<class AFPPlayerCharacter> ChachedFirstPersonCharacterOwner;

private:

	float CalculateCameraPitchAngle() const;
	
};
