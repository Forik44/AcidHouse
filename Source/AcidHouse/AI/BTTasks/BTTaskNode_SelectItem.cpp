#include "AI/BTTasks/BTTaskNode_SelectItem.h"
#include "AIModule/Classes/AIController.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "Characters/AHBaseCharacter.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"

UBTTaskNode_SelectItem::UBTTaskNode_SelectItem()
{
	NodeName = "Select item for shooting";
}

EBTNodeResult::Type UBTTaskNode_SelectItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	checkf(Pawn->IsA<AAHBaseCharacter>(), TEXT("EBTNodeResult::Type UBTTaskNode_SelectItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) working only with AAHBaseCharacter"));
	AAHBaseCharacter* BaseCharacter = StaticCast<AAHBaseCharacter*>(Pawn);

	UCharacterEquipmentComponent* CharacterEquipmentComponent = BaseCharacter->GetCharacterEquipmentComponent_Mutable();
	if (!IsValid(CharacterEquipmentComponent))
	{
		return EBTNodeResult::Failed;
	}

	CharacterEquipmentComponent->EquipItemInSlot(EEquipmentSlots::SecondaryWeapon);
	Blackboard->SetValueAsInt(CurrentItemSlotKey.SelectedKeyName, (uint32)EEquipmentSlots::SecondaryWeapon);
	return EBTNodeResult::Succeeded;
}
