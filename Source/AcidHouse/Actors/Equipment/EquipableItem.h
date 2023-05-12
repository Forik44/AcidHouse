 #pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AcidHouseTypes.h"
#include "EquipableItem.generated.h"

UCLASS(Abstract, NotBlueprintable)
class ACIDHOUSE_API AEquipableItem : public AActor
{
	GENERATED_BODY()

public:
	EEquipableItemType GetItemType() const { return ItemType; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipableItem")
	EEquipableItemType ItemType = EEquipableItemType::None;

};
