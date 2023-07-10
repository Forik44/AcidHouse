#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AHAIController.generated.h"

class UAISense;
UCLASS()
class ACIDHOUSE_API AAHAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AAHAIController();

protected:
	AActor* GetClosestSensedActor(TSubclassOf<UAISense> SenseClass) const;
};
