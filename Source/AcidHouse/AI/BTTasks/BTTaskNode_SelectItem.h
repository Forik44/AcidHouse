#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_SelectItem.generated.h"

UCLASS()
class ACIDHOUSE_API UBTTaskNode_SelectItem : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_SelectItem();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	FBlackboardKeySelector CurrentItemSlotKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	FBlackboardKeySelector TargetKey;
};
