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

void UCharacterEquipmentComponent::Fire()
{
	if (IsValid(CurrentEquipmentWeapon))
	{
		CurrentEquipmentWeapon->Fire();
	}
}

void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(GetOwner()->IsA<AAHBaseCharacter>(), TEXT("UCharacterEquipmentComponent::BeginPlay() CharacterEquipmentComponent can be used only with BaseCharacter"));
	CachedBaseCharacter = StaticCast<AAHBaseCharacter*>(GetOwner());
	CreateLoadout();
}

void UCharacterEquipmentComponent::CreateLoadout()
{
	if (!IsValid(SideArmClass))
	{
		return;
	}
	CurrentEquipmentWeapon = GetWorld()->SpawnActor<ARangeWeapon>(SideArmClass);
	CurrentEquipmentWeapon->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketCharacterWeapon);
	CurrentEquipmentWeapon->SetOwner(CachedBaseCharacter.Get());
}


