// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/MainMenu/NetworkWidget.h"

FText UNetworkWidget::GetNetworkType() const
{
	return bIsLan ? FText::FromString(TEXT("LAN")) : FText::FromString(TEXT("Internet"));
}

void UNetworkWidget::ToggleNetworkType()
{
	bIsLan = !bIsLan;
}

void UNetworkWidget::CloseWidget()
{
	if (OnNetworkWidgetClosed.IsBound())
	{
		OnNetworkWidgetClosed.Broadcast();
	}
	SetVisibility(ESlateVisibility::Hidden);
}
