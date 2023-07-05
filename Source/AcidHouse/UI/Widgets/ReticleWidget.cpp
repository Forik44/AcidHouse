#include "UI/Widgets/ReticleWidget.h"
#include "Actors/Equipment/EquipableItem.h"

void UReticleWidget::OnAimingStateChanged_Implementation(bool bIsAiming)
{
	SetupCurrentReticle();
}

void UReticleWidget::OnEquippedItemChanged_Implementation(const AEquipableItem* EquippedItem)
{
	CurrentEquippedItem = EquippedItem;
	SetupCurrentReticle();
}

void UReticleWidget::SetupCurrentReticle()
{
	CurrentReticle = CurrentEquippedItem.IsValid() ? CurrentEquippedItem->GetReticleType() : EReticleType::None;
}
