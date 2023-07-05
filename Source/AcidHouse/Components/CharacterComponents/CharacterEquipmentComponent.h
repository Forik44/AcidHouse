#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AcidHouseTypes.h"
#include "CharacterEquipmentComponent.generated.h"

typedef TArray<class AEquipableItem*, TInlineAllocator<(uint32)EEquipmentSlots::MAX>> TItemsArray;
typedef TArray<int32, TInlineAllocator<(uint32)EAmunitionType::MAX>> TAmunitionArray;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponAmmoChanged, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentThrowableItemAmmoChanged, int32);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedItemChanged, const AEquipableItem*)

class ARangeWeapon;
class AThrowableItem;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACIDHOUSE_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	EEquipableItemType GetCurrentEquippedItemType() const;

	ARangeWeapon* GetCurrentRangeWeapon() const { return CurrentEquipmentWeapon; }
	AThrowableItem* GetCurrentThrowableItem() const { return CurrentThrowableItem; }

	FOnCurrentWeaponAmmoChanged OnCurrentWeaponAmmoChangedEvent;
	FOnCurrentThrowableItemAmmoChanged OnCurrentThrowableItemAmmoChanged;

	FOnEquippedItemChanged OnEquippedItemChanged;

	void ReloadCurrentWeapon();
	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo = 0, bool bCheckIsFull = false);

	void EquipItemInSlot(EEquipmentSlots Slot);

	void AttachCurrentItemToEquippedSocket();

	void LaunchCurrentThrowableItem();

	void UnequipCurrentItem();

	void EquipNextItem();
	void EquipPreviousItem();

	FORCEINLINE bool IsEquipping() const { return bIsEquipping; }

	int32 GetAmmoCurrentThrowableItem();
	void SetAmmoCurrentThrowableItem(int32 Ammo);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EAmunitionType, int32> MaxAmunitionAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EEquipmentSlots, TSubclassOf<class AEquipableItem>> ItemsLodout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSet<EEquipmentSlots> IgnoreSlotsWhileSwitching;

private:
	bool bIsEquipping = false;

	FTimerHandle EquipTimer;

	TAmunitionArray AmunitionArray;
	TItemsArray ItemsArray;

	ARangeWeapon* CurrentEquipmentWeapon;
	AEquipableItem* CurrentEquippedItem;
	AThrowableItem* CurrentThrowableItem;

	EEquipmentSlots CurrentEquippedSlot;
	EEquipmentSlots PreviousEquippedSlot;

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
