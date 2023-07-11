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

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float TargetReachRadius = 100.0f;

	virtual void BeginPlay() override;
private:
	TWeakObjectPtr<AAHAICharacter> CachedAICharacter;

	bool bIsPatrolling = false;

	void TryMoveToNextTarget();
	bool IsTargetReached(FVector TargetLocation) const;
};
