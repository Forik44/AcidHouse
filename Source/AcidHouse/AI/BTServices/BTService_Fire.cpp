#include "AI/BTServices/BTService_Fire.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/AHBaseCharacter.h"
#include "Actors/Equipment/Weapon/RangeWeapon.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"

UBTService_Fire::UBTService_Fire()
{
	NodeName = "Fire";
}

void UBTService_Fire::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!IsValid(AIController) || !IsValid(Blackboard))
	{
		return;
	}

	checkf(AIController->GetPawn()->IsA<AAHBaseCharacter>(), TEXT("UBTService_Fire::TickNode work only when owner is an AAHBaseCharacter"));
	AAHBaseCharacter* Character = StaticCast<AAHBaseCharacter*>(AIController->GetPawn());

	if (!IsValid(Character))
	{
		return;
	}

	const UCharacterEquipmentComponent* EquipmentComponent = Character->GetCharacterEquipmentComponent();
	ARangeWeapon* RangeWeapon = EquipmentComponent->GetCurrentRangeWeapon();
	if (!IsValid(RangeWeapon))
	{
		return;
	}

	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!IsValid(CurrentTarget))
	{
		Character->StopFire();
		return;
	}

	float DistSq = FVector::DistSquared(CurrentTarget->GetActorLocation(), Character->GetActorLocation());
	if (DistSq > FMath::Square(MaxFireDistance))
	{
		Character->StopFire();
		return;
	}

	if (!RangeWeapon->IsReloading() && !RangeWeapon->IsFiring())
	{
		Character->StartFire() ;
	}
}
