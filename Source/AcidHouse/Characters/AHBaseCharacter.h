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

	virtual void MoveForward(float Value) {};
	virtual void MoveRight(float Value) {};
	virtual void Turn(float Value) {};
	virtual void LookUp(float Value) {};

	virtual void ChangeCrouchState();

	virtual void StartSprint();
	virtual void StopSprint();

	virtual void Tick(float DeltaTime) override;

	FORCEINLINE UAHBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() { return AHBaseCharacterMovementComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKRightFootOffset() const { return IKRightFootOffset; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKLeftFootOffset() const { return IKLeftFootOffset; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement")
	float SprintSpeed = 800.0f;

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

	virtual bool CanSprint();
	UAHBaseCharacterMovementComponent* AHBaseCharacterMovementComponent;

private:
	bool bIsSprintRequested = false;

	float IKRightFootOffset = 0.0f;
	float IKLeftFootOffset = 0.0f;
	
	float IKTraceDistance = 0.0f;

	void TryChangeSprintState();

	float GetIKOffsetForASocket(const FName& SocketName);
};
