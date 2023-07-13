#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RandomPointAroundTarget.generated.h"

UCLASS()
class ACIDHOUSE_API UBTTask_RandomPointAroundTarget : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_RandomPointAroundTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float Radius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	FBlackboardKeySelector LocationKey;
};
