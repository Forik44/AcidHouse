#include "UI/Widgets/PlayerHUDWidget.h"
#include "Characters/AHBaseCharacter.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"
#include "UI/Widgets/ReticleWidget.h"
#include "UI/Widgets/AmmoWidget.h"
#include "Blueprint/WidgetTree.h"
#include "UI/Widgets/CharacterAttributesWidget.h"

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
