#include "AI/Controllers/AITurretController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "AI/Characters/Turret.h"
#include "Perception/AISense_Damage.h"

AAITurretController::AAITurretController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("TurretPerception"));
}

void AAITurretController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (IsValid(InPawn))
	{
		checkf(InPawn->IsA<ATurret>(), TEXT("AAITurretController::SetPawn AAITurretController can posses only ATurret"));
		CachedTurret = StaticCast<ATurret*>(InPawn);
	}
	else
	{
		CachedTurret = nullptr;
	}
}

void AAITurretController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	if (!CachedTurret.IsValid())
	{
		return;
	}

	TArray<AActor*> SeenActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), SeenActors);

	AActor* ClosestActor = nullptr;
	float MinSquaredDistance = FLT_MAX;
	FVector TurretLocation = CachedTurret->GetActorLocation();

	for (AActor* SeenActor : SeenActors)
	{
		float CurrentSquaredDistance = (TurretLocation - SeenActor->GetActorLocation()).SizeSquared();
		if (CurrentSquaredDistance < MinSquaredDistance)
		{
			MinSquaredDistance = CurrentSquaredDistance;
			ClosestActor = SeenActor;
		}
	}

	TArray<AActor*> DamagingActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Damage::StaticClass(), DamagingActors);
	for (AActor* DamagingActor : DamagingActors)
	{
		float CurrentSquaredDistance = (TurretLocation - DamagingActor->GetActorLocation()).SizeSquared();
		if (CurrentSquaredDistance < MinSquaredDistance)
		{
			MinSquaredDistance = CurrentSquaredDistance;
			ClosestActor = DamagingActor;
		}
	}

	CachedTurret->SetCurrentTarget(ClosestActor);
}
