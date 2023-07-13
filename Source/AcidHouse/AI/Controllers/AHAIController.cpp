#include "AI/Controllers/AHAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"

AAHAIController::AAHAIController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

AActor* AAHAIController::GetClosestSensedActor(TSubclassOf<UAISense> SenseClass) const
{
	if (!IsValid(GetPawn()))
	{
		return nullptr;
	}

	TArray<AActor*> SensedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(SenseClass, SensedActors);

	AActor* ClosestActor = nullptr;
	float MinSquaredDistance = FLT_MAX;
	FVector PawnLocation = GetPawn()->GetActorLocation();

	for (AActor* SensedActor : SensedActors)
	{
		float CurrentSquaredDistance = (PawnLocation - SensedActor->GetActorLocation()).SizeSquared();
		if (CurrentSquaredDistance < MinSquaredDistance)
		{
			MinSquaredDistance = CurrentSquaredDistance;
			ClosestActor = SensedActor;
		}
	}

	return ClosestActor;
}
