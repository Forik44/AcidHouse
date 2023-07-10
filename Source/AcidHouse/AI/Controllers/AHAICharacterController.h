#pragma once

#include "CoreMinimal.h"
#include "AI/Controllers/AHAIController.h"
#include "AHAICharacterController.generated.h"


class AAHAICharacter;
UCLASS()
class ACIDHOUSE_API AAHAICharacterController : public AAHAIController
{
	GENERATED_BODY()
	
public:
	virtual void SetPawn(APawn* InPawn) override;

	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;

private:
	TWeakObjectPtr<AAHAICharacter> CachedAICharacter;
};
