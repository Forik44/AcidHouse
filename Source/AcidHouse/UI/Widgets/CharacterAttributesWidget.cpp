#include "UI/Widgets/CharacterAttributesWidget.h"

void UCharacterAttributesWidget::OnHealthPersentChanged(float NewHealthPersent)
{
	HealthPersent = NewHealthPersent;
}

void UCharacterAttributesWidget::OnStaminaPersentChanged(float NewStaminaPersent)
{
	StaminaPersent = NewStaminaPersent;
}

void UCharacterAttributesWidget::OnOxygenPersentChanged(float NewOxygenPersent)
{
	OxygenPersent = NewOxygenPersent;
}
