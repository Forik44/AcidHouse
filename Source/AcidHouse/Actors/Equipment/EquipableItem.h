 #pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AcidHouseTypes.h"
#include "EquipableItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentStateChanged, bool, bIsEquipped);

class UAnimMontage;
class AAHBaseCharacter;
UCLASS(Abstract, NotBlueprintable)
class ACIDHOUSE_API AEquipableItem : public AActor
{
	GENERATED_BODY()

public:
	AEquipableItem();



	virtual void SetOwner(AActor* NewOwner) override;

	EEquipableItemType GetItemType() const { return ItemType; }

	UAnimMontage* GetCharacterEquipAnimMontage() const { return CharacterEquipAnimMontage; };

	FORCEINLINE FName GetUnEquippedSocketName() const { return UnEquippedSocketName; }
	FORCEINLINE FName GetEquippedSocketName() const { return EquippedSocketName; }

	virtual void Equip();
	virtual void UnEquip();

	virtual EReticleType GetReticleType() const;

protected:
	UPROPERTY(BlueprintAssignable)
	FOnEquipmentStateChanged OnEquipmentStateChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipableItem")
	EEquipableItemType ItemType = EEquipableItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipableItem")
	UAnimMontage* CharacterEquipAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipableItem")
	FName UnEquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipableItem")
	FName EquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle")
	EReticleType ReticleType = EReticleType::None;

	AAHBaseCharacter* GetCharacterOwner() const;

private:
	TWeakObjectPtr<AAHBaseCharacter> CachedCharacterOwner;
};
