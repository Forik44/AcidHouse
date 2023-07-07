#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AITurretController.generated.h"

class ATurret;
UCLASS()
class ACIDHOUSE_API AAITurretController : public AAIController
{
	GENERATED_BODY()

public:
	AAITurretController();

	virtual void SetPawn(APawn* InPawn) override;

	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

private:
	TWeakObjectPtr<ATurret> CachedTurret;
	
};
