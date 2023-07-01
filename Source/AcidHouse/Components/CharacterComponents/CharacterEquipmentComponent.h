#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AcidHouseTypes.h"
#include "CharacterEquipmentComponent.generated.h"

typedef TArray<class AEquipableItem*, TInlineAllocator<(uint32)EEquipmentSlots::MAX>> TItemsArray;
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
	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo = 0, bool bCheckIsFull = false);

	void EquipItemInSlot(EEquipmentSlots Slot);

	void AttachCurrentItemToEquippedSocket();

	void UnequipCurrentItem();

	void EquipNextItem();
	void EquipPreviousItem();

	FORCEINLINE bool IsEquipping() const { return bIsEquipping; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EAmunitionType, int32> MaxAmunitionAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EEquipmentSlots, TSubclassOf<class AEquipableItem>> ItemsLodout;

private:
	bool bIsEquipping = false;

	FTimerHandle EquipTimer;

	TAmunitionArray AmunitionArray;
	TItemsArray ItemsArray;

	ARangeWeapon* CurrentEquipmentWeapon;
	AEquipableItem* CurrentEquippedItem;
	EEquipmentSlots CurrentEquippedSlot;

	TWeakObjectPtr<class AAHBaseCharacter> CachedBaseCharacter;

	FDelegateHandle OnCurrentWeaponAmmoChangedHanlde;
	FDelegateHandle OnCurrentWeaponReloadedHanlde;

	int32 GetAvailableAmunitionForCurrentWeapon();

	UFUNCTION()
	void OnCurrentWeaponAmmoChanged(int32 Ammo);

	UFUNCTION()
	void OnWeaponReloadComplete();

	void EquipAnimationFinished();

	void CreateLoadout();

	uint32 NextItemsArraySlotIndex(uint32 CurrentSlotIndex);
	uint32 PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex);
};
