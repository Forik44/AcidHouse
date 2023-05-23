// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/PlayerHUDWidget.h"
#include "Characters/AHBaseCharacter.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"

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
