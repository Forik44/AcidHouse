#pragma once

#include "CoreMinimal.h"
#include "Characters/AHBaseCharacter.h"
#include "AHAICharacter.generated.h"

class UAIPatrollingComponent;
UCLASS(Blueprintable)
class ACIDHOUSE_API AAHAICharacter : public AAHBaseCharacter
{
	GENERATED_BODY()

public:
	AAHAICharacter(const FObjectInitializer& ObjectInitializer);

	UAIPatrollingComponent* GetPatrollingComponent() { return AIPatrollingComponent; }
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIPatrollingComponent* AIPatrollingComponent;
	
};
