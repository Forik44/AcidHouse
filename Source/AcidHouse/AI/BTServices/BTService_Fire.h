// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Fire.generated.h"

UCLASS()
class ACIDHOUSE_API UBTService_Fire : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_Fire();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float MaxFireDistance = 1200.0f;
};
