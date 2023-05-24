#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AcidHouseTypes.h"
#include "CharacterEquipmentComponent.generated.h"

typedef TArray<int32, TInlineAllocator<(uint32)EAmunitionType::MAX>> TAmunitionArray;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponAmmoChanged, int32, int32);

class ARangeWeapon;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACIDHOUSE_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	EEquipableItemType GetCurrentEquippedItemType() const;

	ARangeWeapon* GetCurrentRangeWeapon() const { return CurrentEquipmentWeapon; }

	FOnCurrentWeaponAmmoChanged OnCurrentWeaponAmmoChangedEvent;

	void ReloadCurrentWeapon();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSubclassOf<ARangeWeapon> SideArmClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EAmunitionType, int32> MaxAmunitionAmount;

private:
	TAmunitionArray AmunitionArray;

	ARangeWeapon* CurrentEquipmentWeapon;
	TWeakObjectPtr<class AAHBaseCharacter> CachedBaseCharacter;

	int32 GetAvailableAmunitionForCurrentWeapon();

	UFUNCTION()
	void OnCurrentWeaponAmmoChanged(int32 Ammo);

	void CreateLoadout();

};
