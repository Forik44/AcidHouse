 #pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AcidHouseTypes.h"
#include "EquipableItem.generated.h"

class UAnimMontage;

UCLASS(Abstract, NotBlueprintable)
class ACIDHOUSE_API AEquipableItem : public AActor
{
	GENERATED_BODY()

public:
	EEquipableItemType GetItemType() const { return ItemType; }

	UAnimMontage* GetCharacterEquipAnimMontage() const { return CharacterEquipAnimMontage; };

	FORCEINLINE FName GetUnEquippedSocketName() const { return UnEquippedSocketName; }
	FORCEINLINE FName GetEquippedSocketName() const { return EquippedSocketName; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipableItem")
	EEquipableItemType ItemType = EEquipableItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipableItem")
	UAnimMontage* CharacterEquipAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipableItem")
	FName UnEquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipableItem")
	FName EquippedSocketName = NAME_None;
};
