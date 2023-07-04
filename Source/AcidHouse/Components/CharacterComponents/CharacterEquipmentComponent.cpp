#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Characters/AHBaseCharacter.h"
#include "Actors/Equipment/Weapon/RangeWeapon.h"
#include "AcidHouseTypes.h"
#include "Actors/Equipment/Throwables/ThrowableItem.h"

void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

 	checkf(GetOwner()->IsA<AAHBaseCharacter>(), TEXT("UCharacterEquipmentComponent::BeginPlay() CharacterEquipmentComponent can be used only with BaseCharacter"));
 	CachedBaseCharacter = StaticCast<AAHBaseCharacter*>(GetOwner());
 	CreateLoadout();
}

EEquipableItemType UCharacterEquipmentComponent::GetCurrentEquippedItemType() const
{
	EEquipableItemType Result = EEquipableItemType::None;
	if (IsValid(CurrentEquipmentWeapon))
	{
		Result = CurrentEquipmentWeapon->GetItemType();
	}
	return Result;
}

void UCharacterEquipmentComponent::ReloadCurrentWeapon()
{
	checkf(IsValid(CurrentEquipmentWeapon), TEXT("UCharacterEquipmentComponent::ReloadCurrentWeapon() CurrentEquipmentWeapon doesn't define"));
	int32 AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();
	if (AvailableAmunition <= 0)
	{
		return;
	}

	CurrentEquipmentWeapon->StartReload();
}

void UCharacterEquipmentComponent::ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo /*= 0*/, bool bCheckIsFull /*= false*/)
{
	int32 AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();
	int32 CurrentAmmo = CurrentEquipmentWeapon->GetAmmo();
	int32 AmmoToReload = CurrentEquipmentWeapon->GetMaxAmmo() - CurrentAmmo;

	int32 ReloadedAmmo = FMath::Min(AvailableAmunition, AmmoToReload);
	if (NumberOfAmmo > 0)
	{
		ReloadedAmmo = FMath::Min(ReloadedAmmo, NumberOfAmmo); 
	}

	AmunitionArray[(uint32)CurrentEquipmentWeapon->GetAmmoType()] -= ReloadedAmmo;

	CurrentEquipmentWeapon->SetAmmo(ReloadedAmmo + CurrentAmmo);

	if (bCheckIsFull)
	{
		AvailableAmunition = AmunitionArray[(uint32)CurrentEquipmentWeapon->GetAmmoType()];

		bool bIsFullyReloaded = CurrentEquipmentWeapon->GetAmmo() == CurrentEquipmentWeapon->GetMaxAmmo();
		if (AvailableAmunition == 0 || bIsFullyReloaded)
		{
			CurrentEquipmentWeapon->EndReload(true);
		}
	}
}

void UCharacterEquipmentComponent::EquipItemInSlot(EEquipmentSlots Slot)
{
	if (bIsEquipping)
	{
		return;
	}

	UnequipCurrentItem(); 

	CurrentEquippedItem = ItemsArray[(uint32)Slot];
	CurrentEquipmentWeapon = Cast<ARangeWeapon>(CurrentEquippedItem);
	CurrentThrowableItem = Cast<AThrowableItem>(CurrentEquippedItem);

	if (IsValid(CurrentEquippedItem))
	{
		UAnimMontage* EquipMontage = CurrentEquippedItem->GetCharacterEquipAnimMontage();
		if (IsValid(EquipMontage))
		{
			if ((IsValid(CurrentThrowableItem) && GetAmmoCurrentThrowableItem() > 0) || !IsValid(CurrentThrowableItem))
			{
				bIsEquipping = true;
				float EquipDuration = CachedBaseCharacter->PlayAnimMontage(EquipMontage);
				GetWorld()->GetTimerManager().SetTimer(EquipTimer, this, &UCharacterEquipmentComponent::EquipAnimationFinished, EquipDuration, false);
				CurrentEquippedItem->Equip();
			}
		}
		else
		{
			AttachCurrentItemToEquippedSocket(); 
			CurrentEquippedItem->Equip();
		}
		CurrentEquippedSlot = Slot;
	}

	if (IsValid(CurrentEquipmentWeapon))
	{
		OnCurrentWeaponAmmoChangedHanlde = CurrentEquipmentWeapon->OnAmmoChanged.AddUFunction(this, FName("OnCurrentWeaponAmmoChanged"));
		OnCurrentWeaponReloadedHanlde = CurrentEquipmentWeapon->OnReloadComplete.AddUFunction(this, FName("OnWeaponReloadComplete"));
		OnCurrentWeaponAmmoChanged(CurrentEquipmentWeapon->GetAmmo());
	}
}

void UCharacterEquipmentComponent::AttachCurrentItemToEquippedSocket()
{
	if (!IsValid(CurrentEquippedItem))
	{
		return;
	}
	CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetEquippedSocketName());
}

void UCharacterEquipmentComponent::LaunchCurrentThrowableItem()
{
	if (CurrentThrowableItem)
	{
		CurrentThrowableItem->Throw();
		bIsEquipping = false;
		EquipItemInSlot(PreviousEquippedSlot);
	}
}

void UCharacterEquipmentComponent::UnequipCurrentItem()
{
	if (IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetUnEquippedSocketName());
		CurrentEquippedItem->UnEquip();
	}
	if (IsValid(CurrentEquipmentWeapon))
	{
		CurrentEquipmentWeapon->StopFire();
		CurrentEquipmentWeapon->EndReload(false);
		CurrentEquipmentWeapon->OnAmmoChanged.Remove(OnCurrentWeaponAmmoChangedHanlde);
		CurrentEquipmentWeapon->OnReloadComplete.Remove(OnCurrentWeaponReloadedHanlde);
	}
	PreviousEquippedSlot = CurrentEquippedSlot;
	CurrentEquippedSlot = EEquipmentSlots::None;
}

void UCharacterEquipmentComponent::EquipNextItem()
{
	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 NextSlotIndex = NextItemsArraySlotIndex(CurrentSlotIndex);
	while (CurrentSlotIndex != NextSlotIndex && IgnoreSlotsWhileSwitching.Contains((EEquipmentSlots)NextSlotIndex) && !IsValid(ItemsArray[NextSlotIndex]))
	{
		NextSlotIndex = NextItemsArraySlotIndex(NextSlotIndex);
	}
	if (CurrentSlotIndex != NextSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)NextSlotIndex);
	}
}

void UCharacterEquipmentComponent::EquipPreviousItem()
{
	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 PreviousSlotIndex = PreviousItemsArraySlotIndex(CurrentSlotIndex);
	while (CurrentSlotIndex != PreviousSlotIndex && IgnoreSlotsWhileSwitching.Contains((EEquipmentSlots)PreviousSlotIndex) && !IsValid(ItemsArray[PreviousSlotIndex]))
	{
		PreviousSlotIndex = PreviousItemsArraySlotIndex(PreviousSlotIndex);
	}
	if (CurrentSlotIndex != PreviousSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)PreviousSlotIndex);
	}
}

int32 UCharacterEquipmentComponent::GetAmmoCurrentThrowableItem()
{
	checkf(IsValid(CurrentThrowableItem), TEXT("UCharacterEquipmentComponent::GetAmmoCurrentThrowableItem() CurrentEquipmentItem doesn't define"));
	return AmunitionArray[(uint32)GetCurrentThrowableItem()->GetAmmoType()];
}

void UCharacterEquipmentComponent::SetAmmoCurrentThrowableItem(int32 Ammo)
{
	checkf(IsValid(CurrentThrowableItem), TEXT("UCharacterEquipmentComponent::SetAmmoCurrentThrowableItem CurrentEquipmentItem doesn't define"));
	AmunitionArray[(uint32)GetCurrentThrowableItem()->GetAmmoType()] = Ammo;
	if (OnCurrentThrowableItemAmmoChanged.IsBound())
	{
		OnCurrentThrowableItemAmmoChanged.Broadcast(Ammo);
	}
}

int32 UCharacterEquipmentComponent::GetAvailableAmunitionForCurrentWeapon()
{
	checkf(IsValid(CurrentEquipmentWeapon), TEXT("UCharacterEquipmentComponent::ReloadCurrentWeapon() CurrentEquipmentWeapon doesn't define"));
	return AmunitionArray[(uint32)GetCurrentRangeWeapon()->GetAmmoType()];
}

void UCharacterEquipmentComponent::OnCurrentWeaponAmmoChanged(int32 Ammo)
{
	if (OnCurrentWeaponAmmoChangedEvent.IsBound())
	{
		OnCurrentWeaponAmmoChangedEvent.Broadcast(Ammo, GetAvailableAmunitionForCurrentWeapon());
	}
}

void UCharacterEquipmentComponent::OnWeaponReloadComplete()
{
	ReloadAmmoInCurrentWeapon();
}

void UCharacterEquipmentComponent::EquipAnimationFinished()
{
	bIsEquipping = false;
	AttachCurrentItemToEquippedSocket();
}

void UCharacterEquipmentComponent::CreateLoadout()
{
	AmunitionArray.AddZeroed((uint32)EAmunitionType::MAX);
	for (const TPair<EAmunitionType, int32>& AmmoPair : MaxAmunitionAmount)
	{
		AmunitionArray[(uint32)AmmoPair.Key] = FMath::Max(AmmoPair.Value, 0);
	}

	ItemsArray.AddZeroed((uint32)EEquipmentSlots::MAX);
	for (const TPair<EEquipmentSlots, TSubclassOf<AEquipableItem>>& ItemPair : ItemsLodout)
	{
		if (!IsValid(ItemPair.Value))
		{
			continue;
		}
		AEquipableItem* Item = GetWorld()->SpawnActor<AEquipableItem>(ItemPair.Value);
		Item->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Item->GetUnEquippedSocketName());
		Item->SetOwner(CachedBaseCharacter.Get());
		Item->UnEquip();
		ItemsArray[(uint32)ItemPair.Key] = Item;
		if (Item->IsA<AThrowableItem>())
		{
			AThrowableItem* ThrowableItem = StaticCast<AThrowableItem*>(Item);
			if (OnCurrentThrowableItemAmmoChanged.IsBound())
			{
				OnCurrentThrowableItemAmmoChanged.Broadcast(AmunitionArray[(uint32)ThrowableItem->GetAmmoType()]);
			}
		}
	}
}

uint32 UCharacterEquipmentComponent::NextItemsArraySlotIndex(uint32 CurrentSlotIndex)
{
	if (CurrentSlotIndex == ItemsArray.Num() - 1)
	{
		return 0;
	}
	else
	{
		return CurrentSlotIndex + 1;
	}
}

uint32 UCharacterEquipmentComponent::PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex)
{
	if (CurrentSlotIndex == 0)
	{
		return ItemsArray.Num() - 1;
	}
	else
	{
		return CurrentSlotIndex - 1;
	}
}

