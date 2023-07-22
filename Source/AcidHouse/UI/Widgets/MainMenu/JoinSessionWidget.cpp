#include "UI/Widgets/MainMenu/JoinSessionWidget.h"
#include "Kismet/GameplayStatics.h"
#include "AHGameInstance.h"

void UJoinSessionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());

	checkf(GameInstance->IsA<UAHGameInstance>(), TEXT("UHostSessionWidget::CreateSession() workin only with UAHGameInstance"));
	AHGameInstance = StaticCast<UAHGameInstance*>(GameInstance);
}

void UJoinSessionWidget::FindOnlineSession()
{
	AHGameInstance->OnMatchFound.AddUFunction(this, FName("OnMatchFound"));
	AHGameInstance->FindMatch(bIsLan);
	SearchingSessionState = ESearchingSessionState::Searching;
}

void UJoinSessionWidget::JoinOnlineSession()
{
	AHGameInstance->JoinOnlineGame();
}

void UJoinSessionWidget::OnMatchFound_Implementation(bool bIsSuccesful)
{
	SearchingSessionState = bIsSuccesful ? ESearchingSessionState::SessionIsFound : ESearchingSessionState::None;
	AHGameInstance->OnMatchFound.RemoveAll(this);
}

void UJoinSessionWidget::CloseWidget()
{
	AHGameInstance->OnMatchFound.RemoveAll(this);
	Super::CloseWidget();
}
