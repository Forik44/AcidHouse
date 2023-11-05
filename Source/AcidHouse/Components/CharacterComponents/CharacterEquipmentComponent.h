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
class AMeleeWeapon;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACIDHOUSE_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterEquipmentComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	EEquipableItemType GetCurrentEquippedItemType() const;

	ARangeWeapon* GetCurrentRangeWeapon() const { return CurrentEquippedWeapon; }
	AThrowableItem* GetCurrentThrowableItem() const { return CurrentThrowableItem; }
	AMeleeWeapon* GetCurrentMeleeWeapon() const { return CurrentMeleeWeapon; }

	FOnCurrentWeaponAmmoChanged OnCurrentWeaponAmmoChangedEvent;
	FOnCurrentThrowableItemAmmoChanged OnCurrentThrowableItemAmmoChanged;

	FOnEquippedItemChanged OnEquippedItemChanged;

	void ReloadCurrentWeapon();
	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo = 0, bool bCheckIsFull = false);

	UFUNCTION(BlueprintCallable)
	void EquipItemInSlot(EEquipmentSlots Slot, bool bForce = false);

	void AttachCurrentItemToEquippedSocket();

	void LaunchCurrentThrowableItem();

	void UnequipCurrentItem();

	void EquipNextItem();
	void EquipPreviousItem();

	FORCEINLINE bool IsEquipping() const { return bIsEquipping; }

	EEquipmentSlots GetPreviousEquipmentSlot() const { return PreviousEquippedSlot; }

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	EEquipmentSlots AutoEquipItemInSlot = EEquipmentSlots::None;

private:
	UPROPERTY(ReplicatedUsing = OnRep_bIsReloading)
	bool bIsReloading = false;

	bool bIsEquipping = false;

	FTimerHandle EquipTimer;

	UPROPERTY(Replicated)
	TArray<int32> AmunitionArray;

	UPROPERTY(ReplicatedUsing=OnRep_ItemsArray)
	TArray<AEquipableItem*> ItemsArray;

	ARangeWeapon* CurrentEquippedWeapon;
	AEquipableItem* CurrentEquippedItem;
	AThrowableItem* CurrentThrowableItem;
	AMeleeWeapon* CurrentMeleeWeapon;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentEquippedSlot)
	EEquipmentSlots CurrentEquippedSlot;

	UPROPERTY(ReplicatedUsing=OnRep_PreviousEquippedSlot)
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

	void AutoEquip();

	UFUNCTION(Server, Reliable)
	void Server_EquipItemInSlot(EEquipmentSlots Slot);

	UFUNCTION(Server, Reliable)
	void Server_ReloadCurrentWeapon();

	UFUNCTION(Server, Reliable)
	void Server_SetPreviousEquippedSlot(EEquipmentSlots NewPreviousEquippedSlot);

	UFUNCTION()
	void OnRep_CurrentEquippedSlot(EEquipmentSlots CurrentEquippedSlot_Old);

	UFUNCTION()
	void OnRep_ItemsArray();

	UFUNCTION()
	void OnRep_bIsReloading();

	UFUNCTION()
	void OnRep_PreviousEquippedSlot(EEquipmentSlots PreviousEquippedSlot_Old);

	uint32 NextItemsArraySlotIndex(uint32 CurrentSlotIndex);
	uint32 PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex);
};
