#include "UI/Widgets/PlayerHUDWidget.h"
#include "Characters/AHBaseCharacter.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"
#include "UI/Widgets/ReticleWidget.h"
#include "UI/Widgets/AmmoWidget.h"
#include "Blueprint/WidgetTree.h"
#include "UI/Widgets/CharacterAttributesWidget.h"
#include "HighlightInteractableWidget.h"

class UReticleWidget* UPlayerHUDWidget::GetReticleWidget()
{
	return WidgetTree->FindWidget<UReticleWidget>(ReticleWidgetName);
}

class UAmmoWidget* UPlayerHUDWidget::GetAmmoWidget()
{
	return WidgetTree->FindWidget<UAmmoWidget>(AmmoWidgetName);
}

UCharacterAttributesWidget* UPlayerHUDWidget::GetCharacterAttributesWidget()
{
	return WidgetTree->FindWidget<UCharacterAttributesWidget>(CharacterAttributesName);
}

void UPlayerHUDWidget::SetHighlightInteractableVisability(bool bIsVisible)
{
	if (!IsValid(InteractableKey))
	{
		return;
	}

	if (bIsVisible)
	{
		InteractableKey->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InteractableKey->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPlayerHUDWidget::SetHighlightInteractableActionText(FName KeyName)
{
	if (!IsValid(InteractableKey))
	{
		return;
	}

	InteractableKey->SetActionText(KeyName);
}
