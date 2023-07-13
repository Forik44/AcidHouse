#pragma once

#include "CoreMinimal.h"
#include "AI/Controllers/AHAIController.h"
#include "AITurretController.generated.h"

class ATurret;
UCLASS()
class ACIDHOUSE_API AAITurretController : public AAHAIController
{
	GENERATED_BODY()

public:
	virtual void SetPawn(APawn* InPawn) override;

	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

private:
	TWeakObjectPtr<ATurret> CachedTurret;
	
};
