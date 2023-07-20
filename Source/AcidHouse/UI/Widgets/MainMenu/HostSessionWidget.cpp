// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/MainMenu/HostSessionWidget.h"
#include "Kismet/GameplayStatics.h"
#include "AHGameInstance.h"

void UHostSessionWidget::CreateSession()
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());

	checkf(GameInstance->IsA<UAHGameInstance>(), TEXT("UHostSessionWidget::CreateSession() workin only with UAHGameInstance"));
	UAHGameInstance* AHGameInstance = StaticCast<UAHGameInstance*>(GameInstance);

	//TODO DELETE HARDCODE
	AHGameInstance->LaunchLobby(4, ServerName, bIsLan);
}
