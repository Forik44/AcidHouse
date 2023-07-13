#include "AI/BTTasks/BTTask_RandomPointAroundTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTTask_RandomPointAroundTarget::UBTTask_RandomPointAroundTarget()
{
	NodeName = "Select random point around target";
}

EBTNodeResult::Type UBTTask_RandomPointAroundTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!IsValid(AIController) || !IsValid(Blackboard))
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIController->GetPawn();
	if (!IsValid(Pawn))
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Pawn);
	if (!IsValid(NavSys))
	{
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!IsValid(TargetActor))
	{
		return EBTNodeResult::Failed;
	}

	FNavLocation NavLocation;
	bool bIsFound = NavSys->GetRandomReachablePointInRadius(TargetActor->GetActorLocation(), Radius, NavLocation);

	if (!bIsFound)
	{
		return EBTNodeResult::Failed;
	}

	Blackboard->SetValueAsVector(LocationKey.SelectedKeyName, NavLocation.Location);
	return EBTNodeResult::Succeeded;
}
