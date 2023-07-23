#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Characters/AHBaseCharacter.h"
#include "Actors/Equipment/Weapon/RangeWeapon.h"
#include "Actors/Equipment/Weapon/MeleeWeapon.h"
#include "AcidHouseTypes.h"
#include "Actors/Equipment/Throwables/ThrowableItem.h"
#include "Net/UnrealNetwork.h"

void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

 	checkf(GetOwner()->IsA<AAHBaseCharacter>(), TEXT("UCharacterEquipmentComponent::BeginPlay() CharacterEquipmentComponent can be used only with BaseCharacter"));
 	CachedBaseCharacter = StaticCast<AAHBaseCharacter*>(GetOwner());
 	CreateLoadout();
	AutoEquip();
}

UCharacterEquipmentComponent::UCharacterEquipmentComponent()
{
	SetIsReplicatedByDefault(true);
}

void UCharacterEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCharacterEquipmentComponent, CurrentEquippedSlot);
	DOREPLIFETIME(UCharacterEquipmentComponent, AmunitionArray);
	DOREPLIFETIME(UCharacterEquipmentComponent, ItemsArray);
	DOREPLIFETIME(UCharacterEquipmentComponent, bIsReloading);
	DOREPLIFETIME(UCharacterEquipmentComponent, PreviousEquippedSlot);
}

EEquipableItemType UCharacterEquipmentComponent::GetCurrentEquippedItemType() const
{
	EEquipableItemType Result = EEquipableItemType::None;
	if (IsValid(CurrentEquippedItem))
	{
		Result = CurrentEquippedItem->GetItemType();
	}
	return Result;
}

void UCharacterEquipmentComponent::ReloadCurrentWeapon()
{
	checkf(IsValid(CurrentEquippedWeapon), TEXT("UCharacterEquipmentComponent::ReloadCurrentWeapon() CurrentEquipmentWeapon doesn't define"));

	if (CurrentEquippedWeapon->IsReloading() && GetOwner()->GetLocalRole() >= ROLE_AutonomousProxy)
	{
		return;
	}

	if (bIsReloading && GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		return;
	}

	int32 AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();
	if (AvailableAmunition <= 0)
	{
		return;
	}

	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		bIsReloading = true;
	}

	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_ReloadCurrentWeapon();
	}
	CurrentEquippedWeapon->StartReload();
}

void UCharacterEquipmentComponent::ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo /*= 0*/, bool bCheckIsFull /*= false*/)
{
	int32 AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();
	int32 CurrentAmmo = CurrentEquippedWeapon->GetAmmo();
	int32 AmmoToReload = CurrentEquippedWeapon->GetMaxAmmo() - CurrentAmmo;

	int32 ReloadedAmmo = FMath::Min(AvailableAmunition, AmmoToReload);
	if (NumberOfAmmo > 0)
	{
		ReloadedAmmo = FMath::Min(ReloadedAmmo, NumberOfAmmo); 
	}

	AmunitionArray[(uint32)CurrentEquippedWeapon->GetAmmoType()] -= ReloadedAmmo;

	CurrentEquippedWeapon->SetAmmo(ReloadedAmmo + CurrentAmmo);

	if (bCheckIsFull)
	{
		AvailableAmunition = AmunitionArray[(uint32)CurrentEquippedWeapon->GetAmmoType()];

		bool bIsFullyReloaded = CurrentEquippedWeapon->GetAmmo() == CurrentEquippedWeapon->GetMaxAmmo();
		if (AvailableAmunition == 0 || bIsFullyReloaded)
		{
			CurrentEquippedWeapon->EndReload(true);
		}
	}
}

void UCharacterEquipmentComponent::EquipItemInSlot(EEquipmentSlots Slot)
{
	if (bIsEquipping)
	{
		return;
	}

	AEquipableItem* Item = ItemsArray[(uint32)Slot];

	CurrentThrowableItem = Cast<AThrowableItem>(Item);
	if ((IsValid(CurrentThrowableItem) && GetAmmoCurrentThrowableItem() <= 0))
	{
		return;
	}

	UnequipCurrentItem();
	CurrentEquippedItem = Item;
	CurrentEquippedWeapon = Cast<ARangeWeapon>(Item);
	CurrentMeleeWeapon = Cast<AMeleeWeapon>(Item);

	if (IsValid(CurrentEquippedItem))
	{
		UAnimMontage* EquipMontage = CurrentEquippedItem->GetCharacterEquipAnimMontage();
		if (IsValid(EquipMontage))
		{
			bIsEquipping = true;
			float EquipDuration = CachedBaseCharacter->PlayAnimMontage(EquipMontage);
			GetWorld()->GetTimerManager().SetTimer(EquipTimer, this, &UCharacterEquipmentComponent::EquipAnimationFinished, EquipDuration, false);

			if (CachedBaseCharacter->IsLocallyControlled())
			{
				CurrentEquippedItem->Equip();
			}
		}
		else
		{
			AttachCurrentItemToEquippedSocket();
			if (CachedBaseCharacter->IsLocallyControlled())
			{
				CurrentEquippedItem->Equip();
			}
		}
	}

	if (IsValid(CurrentEquippedWeapon))
	{
		OnCurrentWeaponAmmoChangedHanlde = CurrentEquippedWeapon->OnAmmoChanged.AddUFunction(this, FName("OnCurrentWeaponAmmoChanged"));
		OnCurrentWeaponReloadedHanlde = CurrentEquippedWeapon->OnReloadComplete.AddUFunction(this, FName("OnWeaponReloadComplete"));
		OnCurrentWeaponAmmoChanged(CurrentEquippedWeapon->GetAmmo());
	}

	if (OnEquippedItemChanged.IsBound())
	{
		OnEquippedItemChanged.Broadcast(CurrentEquippedItem);
	}

	CurrentEquippedSlot = Slot;
	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_EquipItemInSlot(CurrentEquippedSlot);
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
	if (CurrentThrowableItem && CachedBaseCharacter->IsLocallyControlled())
	{
		CurrentThrowableItem->Throw();
	}
	bIsEquipping = false;
	EquipItemInSlot(PreviousEquippedSlot);
}

void UCharacterEquipmentComponent::UnequipCurrentItem()
{
	if (IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetUnEquippedSocketName());
		CurrentEquippedItem->UnEquip();
		CurrentEquippedItem = nullptr;
	}
	if (IsValid(CurrentEquippedWeapon))
	{
		CurrentEquippedWeapon->StopFire();
		CurrentEquippedWeapon->EndReload(false);
		CurrentEquippedWeapon->OnAmmoChanged.Remove(OnCurrentWeaponAmmoChangedHanlde);
		CurrentEquippedWeapon->OnReloadComplete.Remove(OnCurrentWeaponReloadedHanlde);
		CurrentEquippedWeapon = nullptr;
		if (CachedBaseCharacter->IsLocallyControlled())
		{
			if (!CachedBaseCharacter->HasAuthority())
			{
				Server_SetPreviousEquippedSlot(CurrentEquippedSlot);
			}
			PreviousEquippedSlot = CurrentEquippedSlot;
		}
	}
	if (IsValid(CurrentMeleeWeapon))
	{
		if (CachedBaseCharacter->IsLocallyControlled())
		{
			if (!CachedBaseCharacter->HasAuthority())
			{
				Server_SetPreviousEquippedSlot(CurrentEquippedSlot);
			}
			PreviousEquippedSlot = CurrentEquippedSlot;
		}
		CurrentMeleeWeapon = nullptr;
	}
	CurrentEquippedSlot = EEquipmentSlots::None;
}

void UCharacterEquipmentComponent::EquipNextItem()
{
	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 NextSlotIndex = NextItemsArraySlotIndex(CurrentSlotIndex);
	while (CurrentSlotIndex != NextSlotIndex && (IgnoreSlotsWhileSwitching.Contains((EEquipmentSlots)NextSlotIndex) || !IsValid(ItemsArray[NextSlotIndex])))
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
	while (CurrentSlotIndex != PreviousSlotIndex  && (IgnoreSlotsWhileSwitching.Contains((EEquipmentSlots)PreviousSlotIndex) || !IsValid(ItemsArray[PreviousSlotIndex])))
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

void UCharacterEquipmentComponent::OnRep_CurrentEquippedSlot(EEquipmentSlots CurrentEquippedSlot_Old)
{
	EquipItemInSlot(CurrentEquippedSlot);
}

void UCharacterEquipmentComponent::OnRep_ItemsArray()
{
	for (AEquipableItem* Item : ItemsArray)
	{
		if (IsValid(Item))
		{
			Item->UnEquip();
		}
	}
}

void UCharacterEquipmentComponent::OnRep_bIsReloading()
{
	bIsReloading = false;
	ReloadCurrentWeapon();
}

int32 UCharacterEquipmentComponent::GetAvailableAmunitionForCurrentWeapon()
{
	checkf(IsValid(CurrentEquippedWeapon), TEXT("UCharacterEquipmentComponent::ReloadCurrentWeapon() CurrentEquipmentWeapon doesn't define"));
	return AmunitionArray[(uint32)GetCurrentRangeWeapon()->GetAmmoType()];
}

void UCharacterEquipmentComponent::OnRep_PreviousEquippedSlot(EEquipmentSlots PreviousEquippedSlot_Old)
{
	//PreviousEquippedSlot = PreviousEquippedSlot_Old;
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
	bIsReloading = false;
}

void UCharacterEquipmentComponent::EquipAnimationFinished()
{
	bIsEquipping = false;
	AttachCurrentItemToEquippedSocket();
}

void UCharacterEquipmentComponent::CreateLoadout()
{
	AActor* Owner = GetOwner();
	ENetRole NetRole = Owner->GetLocalRole();
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

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
		Item->SetOwner(GetOwner());
		Item->UnEquip();
		ItemsArray[(uint32)ItemPair.Key] = Item;
		if (Item->IsA<AThrowableItem>())
		{
			AThrowableItem* ThrowableItem = StaticCast<AThrowableItem*>(Item);
			ThrowableItem->Initialize();
			if (OnCurrentThrowableItemAmmoChanged.IsBound())
			{
				OnCurrentThrowableItemAmmoChanged.Broadcast(AmunitionArray[(uint32)ThrowableItem->GetAmmoType()]);
			}
		}
	}
}

void UCharacterEquipmentComponent::AutoEquip()
{
	if (AutoEquipItemInSlot != EEquipmentSlots::None)
	{
		EquipItemInSlot(AutoEquipItemInSlot);
	}
}

void UCharacterEquipmentComponent::Server_EquipItemInSlot_Implementation(EEquipmentSlots Slot)
{
	EquipItemInSlot(Slot);
}

void UCharacterEquipmentComponent::Server_ReloadCurrentWeapon_Implementation()
{
	ReloadCurrentWeapon();
}

void UCharacterEquipmentComponent::Server_SetPreviousEquippedSlot_Implementation(EEquipmentSlots NewPreviousEquippedSlot)
{
	PreviousEquippedSlot = NewPreviousEquippedSlot;
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

