#include "UI/Widgets/CharacterAttrebutesWidget.h"

void UCharacterAttrebutesWidget::OnHealthPersentChanged(float NewHealthPersent)
{
	HealthPersent = NewHealthPersent;
}

void UCharacterAttrebutesWidget::OnStaminaPersentChanged(float NewStaminaPersent)
{
	StaminaPersent = NewStaminaPersent;
}

void UCharacterAttrebutesWidget::OnOxygenPersentChanged(float NewOxygenPersent)
{
	OxygenPersent = NewOxygenPersent;
}
