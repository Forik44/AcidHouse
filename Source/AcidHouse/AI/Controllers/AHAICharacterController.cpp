#include "AI/Controllers/AHAICharacterController.h"
#include "AI/Characters/AHAICharacter.h"
#include "Perception/AISense_Sight.h"
#include "Components/CharacterComponents/AIPatrollingComponent.h"

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
	TryMoveToNextTarget(); 
} 

void AAHAICharacterController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	if (!Result.IsSuccess())
	{
		return;
	}
	TryMoveToNextTarget();
}

void AAHAICharacterController::BeginPlay()
{
	Super::BeginPlay();
	
	UAIPatrollingComponent* PatrollingComponent = CachedAICharacter->GetPatrollingComponent();
	if (PatrollingComponent->CanPatrol())
	{
		FVector ClosestWayPoint = PatrollingComponent->SelectClosestWayPoint();
		MoveToLocation(ClosestWayPoint);
		bIsPatrolling = true;
	}
}

void AAHAICharacterController::TryMoveToNextTarget()
{
	UAIPatrollingComponent* PatrollingComponent = CachedAICharacter->GetPatrollingComponent();
	AActor* ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	if (IsValid(ClosestActor))
	{
		if (!IsTargetReached(ClosestActor->GetActorLocation()))
		{
			MoveToActor(ClosestActor);
		}
		bIsPatrolling = false; 
	}
	else if (PatrollingComponent->CanPatrol())
	{
		FVector WayPoint = bIsPatrolling ? PatrollingComponent->SelectNextWayPoint() : PatrollingComponent->SelectClosestWayPoint();
		if (!IsTargetReached(WayPoint))
		{
			MoveToLocation(WayPoint);
		}
		bIsPatrolling = true;
	}
}

bool AAHAICharacterController::IsTargetReached(FVector TargetLocation) const
{
	return (TargetLocation - CachedAICharacter->GetActorLocation()).SizeSquared() <= FMath::Square(TargetReachRadius);
}
