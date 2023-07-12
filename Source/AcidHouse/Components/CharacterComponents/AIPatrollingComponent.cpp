#include "Components/CharacterComponents/AIPatrollingComponent.h"
#include "Actors/Navigation/PatrollingPath.h"

bool UAIPatrollingComponent::CanPatrol() const
{
	return IsValid(PatrollingPath) && PatrollingPath->GetWayPoints().Num() > 0;
}

FVector UAIPatrollingComponent::SelectClosestWayPoint()
{
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	const TArray<FVector> WayPoints = PatrollingPath->GetWayPoints();
	FTransform PathTransform = PatrollingPath->GetActorTransform();

	FVector ClosestWayPoint;
	float MinSqDistance = FLT_MAX;
	for (int32 i = 0; i < WayPoints.Num(); ++i)
	{
		FVector WayPointWorld = PathTransform.TransformPosition(WayPoints[i]);
		float CurrentSqDistance = (OwnerLocation - WayPointWorld).SizeSquared();
		if (CurrentSqDistance < MinSqDistance)
		{
			MinSqDistance = CurrentSqDistance;
			ClosestWayPoint = WayPointWorld; 
			CurrentWayPointIndex = i;
		}
	}

	return ClosestWayPoint;
}

FVector UAIPatrollingComponent::SelectNextWayPoint()
{
	const TArray<FVector> WayPoints = PatrollingPath->GetWayPoints();
	switch (PatrollingType)
	{
	case EPatrollingType::None:
		++CurrentWayPointIndex;
		break;
	case EPatrollingType::Circle:
	{
		++CurrentWayPointIndex;
		if (CurrentWayPointIndex == WayPoints.Num() && WayPoints.Num() != 1)
		{
			CurrentWayPointIndex = 0;
		}
		else if (WayPoints.Num() == 1)
		{
			CurrentWayPointIndex = -1;
		}
		break;
	}
	case EPatrollingType::PingPong:
	{
		if (bIsForward)
		{
			++CurrentWayPointIndex;
			if (CurrentWayPointIndex == WayPoints.Num() && WayPoints.Num() != 1)
			{
				bIsForward = false;
				CurrentWayPointIndex -= 2;
			}
			else if (WayPoints.Num() == 1)
			{
				CurrentWayPointIndex = -1;
			}
		}
		else
		{
			--CurrentWayPointIndex;
			if (CurrentWayPointIndex == -1 && WayPoints.Num() != 1)
			{
				bIsForward = true;
				CurrentWayPointIndex += 2;
			}
			else if (WayPoints.Num() == 1)
			{
				CurrentWayPointIndex = -1;
			}
		}
		break;
	}
	default:
		break;
	}
	
	
	FTransform PathTransform = PatrollingPath->GetActorTransform();
	FVector WayPoint = PathTransform.TransformPosition(WayPoints[CurrentWayPointIndex]);
	return WayPoint;
}

