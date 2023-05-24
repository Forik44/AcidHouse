#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Characters/AHBaseCharacter.h"
#include "Actors/Equipment/Weapon/RangeWeapon.h"
#include "AcidHouseTypes.h"

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

void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(GetOwner()->IsA<AAHBaseCharacter>(), TEXT("UCharacterEquipmentComponent::BeginPlay() CharacterEquipmentComponent can be used only with BaseCharacter"));
	CachedBaseCharacter = StaticCast<AAHBaseCharacter*>(GetOwner());
	CreateLoadout();
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
	int32 AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();
	int32 CurrentAmmo = CurrentEquipmentWeapon->GetAmmo();
	int32 AmmoToReload = CurrentEquipmentWeapon->GetMaxAmmo() - CurrentAmmo;
	int32 ReloadedAmmo = FMath::Min(AvailableAmunition, AmmoToReload);

	AmunitionArray[(uint32)CurrentEquipmentWeapon->GetAmmoType()] -= ReloadedAmmo;

	CurrentEquipmentWeapon->SetAmmo(ReloadedAmmo + CurrentAmmo);
}

void UCharacterEquipmentComponent::CreateLoadout()
{
	AmunitionArray.AddZeroed((uint32)EAmunitionType::MAX);
	for (const TPair<EAmunitionType, int32>& AmmoPair : MaxAmunitionAmount)
	{
		AmunitionArray[(uint32)AmmoPair.Key] = FMath::Max(AmmoPair.Value, 0);
	}

	if (!IsValid(SideArmClass))
	{
		return;
	}

	CurrentEquipmentWeapon = GetWorld()->SpawnActor<ARangeWeapon>(SideArmClass);
	CurrentEquipmentWeapon->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketCharacterWeapon);
	CurrentEquipmentWeapon->SetOwner(CachedBaseCharacter.Get());
	CurrentEquipmentWeapon->OnAmmoChanged.AddUFunction(this, FName("OnCurrentWeaponAmmoChanged"));
	CurrentEquipmentWeapon->OnReloadComplete.AddUFunction(this, FName("OnWeaponReloadComplete"));
	OnCurrentWeaponAmmoChanged(CurrentEquipmentWeapon->GetAmmo());
}


