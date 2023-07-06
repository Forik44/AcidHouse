#include "Actors/Equipment/EquipableItem.h"
#include "Characters/AHBaseCharacter.h"

void AEquipableItem::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	if (IsValid(NewOwner))
	{
		checkf(GetOwner()->IsA<AAHBaseCharacter>(), TEXT("AEquipableItem::SetOwner only character can be an owner of an equibable item"));
		CachedCharacterOwner = StaticCast<AAHBaseCharacter*>(GetOwner());
	}
	else
	{
		CachedCharacterOwner = nullptr;
	}
}

void AEquipableItem::Equip()
{
	if (OnEquipmentStateChanged.IsBound())
	{
		OnEquipmentStateChanged.Broadcast(true);
	}
}

void AEquipableItem::UnEquip()
{
	if (OnEquipmentStateChanged.IsBound())
	{
		OnEquipmentStateChanged.Broadcast(false);
	}
}

EReticleType AEquipableItem::GetReticleType() const
{
	return ReticleType;
}

AAHBaseCharacter* AEquipableItem::GetCharacterOwner() const
{
	return CachedCharacterOwner.IsValid() ? CachedCharacterOwner.Get() : nullptr;
}
