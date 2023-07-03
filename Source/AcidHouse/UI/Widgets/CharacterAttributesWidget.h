#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterAttributesWidget.generated.h"


UCLASS()
class ACIDHOUSE_API UCharacterAttributesWidget : public UUserWidget
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
