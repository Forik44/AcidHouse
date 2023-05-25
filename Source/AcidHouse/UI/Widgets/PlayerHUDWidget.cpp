// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PlayerHUDWidget.h"
#include "Characters/AHBaseCharacter.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"
#include "UI/Widgets/ReticleWidget.h"
#include "UI/Widgets/AmmoWidget.h"
#include "Blueprint/WidgetTree.h"

class UReticleWidget* UPlayerHUDWidget::GetReticleWidget()
{
	return WidgetTree->FindWidget<UReticleWidget>(ReticleWidgetName);
}

class UAmmoWidget* UPlayerHUDWidget::GetAmmoWidget()
{
	return WidgetTree->FindWidget<UAmmoWidget>(AmmoWidgetName);
}

float UPlayerHUDWidget::GetHealthPersent() const
{
	float Result = 1.0f;
	APawn* Pawn = GetOwningPlayerPawn();
	AAHBaseCharacter* Character = Cast<AAHBaseCharacter>(Pawn);
	if (IsValid(Character))
	{
		const UCharacterAttributeComponent* AttributeComponent = Character->GetCharacterAttributeComponent();
		Result = AttributeComponent->GetHealthPersent();
	}
	return Result;
}
