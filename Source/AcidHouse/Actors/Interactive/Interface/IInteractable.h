#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IInteractable.generated.h"


class AAHBaseCharacter;
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class ACIDHOUSE_API IInteractable
{
	GENERATED_BODY()

public:
	virtual void Interact(AAHBaseCharacter* Character) PURE_VIRTUAL(IInteractable::Interact, );
	virtual FName GetActionEventName() const PURE_VIRTUAL(IInteractable::GetActionEventName, return FName(NAME_None););
};
