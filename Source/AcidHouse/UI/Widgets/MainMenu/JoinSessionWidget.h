#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/MainMenu/NetworkWidget.h"
#include "JoinSessionWidget.generated.h"

UENUM(BlueprintType)
enum class ESearchingSessionState : uint8
{
	None,
	Searching,
	SessionIsFound
};

class UAHGameInstance;
UCLASS()
class ACIDHOUSE_API UJoinSessionWidget : public UNetworkWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "Network session")
	ESearchingSessionState SearchingSessionState;

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void FindOnlineSession();

	UFUNCTION(BlueprintCallable)
	void JoinOnlineSession();

	UFUNCTION(BlueprintNativeEvent)
	void OnMatchFound(bool bIsSuccesful);

	virtual void CloseWidget() override;

private:
	TWeakObjectPtr<UAHGameInstance> AHGameInstance;
};
