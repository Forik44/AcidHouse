#include "AI/Controllers/AITurretController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "AI/Characters/Turret.h"
#include "Perception/AISense_Damage.h"

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
	Super::ActorsPerceptionUpdated(UpdatedActors);
	if (!CachedTurret.IsValid())
	{
		return;
	}

	AActor* ClosestSightActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	AActor* ClosestDamagingActor = GetClosestSensedActor(UAISense_Damage::StaticClass());
	
	
	if (!IsValid(ClosestSightActor) || !IsValid(ClosestDamagingActor))
	{
		CachedTurret->SetCurrentTarget(IsValid(ClosestSightActor) ? ClosestSightActor : ClosestDamagingActor);
		return;
	}
	
	if ((GetPawn()->GetActorLocation() - ClosestSightActor->GetActorLocation()).SizeSquared() 
		< (GetPawn()->GetActorLocation() - ClosestDamagingActor->GetActorLocation()).SizeSquared())
	{
		CachedTurret->SetCurrentTarget(ClosestSightActor);
	}
	else
	{
		CachedTurret->SetCurrentTarget(ClosestDamagingActor);
	}
}
