#include "AI/Characters/AHAICharacter.h"
#include "Components/CharacterComponents/AIPatrollingComponent.h"

AAHAICharacter::AAHAICharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIPatrollingComponent = CreateDefaultSubobject<UAIPatrollingComponent>(TEXT("UAIPatrolling"));
}

UBehaviorTree* AAHAICharacter::GetBehaviorTree() const
{
	return BehaviorTree;
}
