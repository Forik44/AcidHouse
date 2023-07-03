#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterAttrebutesWidget.generated.h"


UCLASS()
class ACIDHOUSE_API UCharacterAttrebutesWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterAttrebutes")
	float HealthPersent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterAttrebutes")
	float StaminaPersent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterAttrebutes")
	float OxygenPersent;
private:
	UFUNCTION()
	void OnHealthPersentChanged(float NewHealthPersent);

	UFUNCTION()
	void OnStaminaPersentChanged(float NewStaminaPersent);

	UFUNCTION()
	void OnOxygenPersentChanged(float NewOxygenPersent);
	
};
