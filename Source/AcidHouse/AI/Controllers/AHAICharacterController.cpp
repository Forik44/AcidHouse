#include "AI/Controllers/AHAICharacterController.h"
#include "AI/Characters/AHAICharacter.h"
#include "Perception/AISense_Sight.h"

void AAHAICharacterController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (IsValid(InPawn))
	{
		checkf(InPawn->IsA<AAHAICharacter>(), TEXT("AAHAICharacterController::SetPawn AHAICharacterController can posses only AHAICharacter"));
		CachedAICharacter = StaticCast<AAHAICharacter*>(InPawn);
	}
	else
	{
		CachedAICharacter = nullptr;
	}
}

void AAHAICharacterController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);
	if (!CachedAICharacter.IsValid())
	{
		return;
	}
	AActor* ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	if (IsValid(ClosestActor))
	{
		MoveToActor(ClosestActor);
	}
} 
