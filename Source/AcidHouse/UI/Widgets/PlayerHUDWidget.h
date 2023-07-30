#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

class UHighlightInteractableWidget;
UCLASS()
class ACIDHOUSE_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	class UReticleWidget* GetReticleWidget();

	class UAmmoWidget* GetAmmoWidget();
	
	class UCharacterAttributesWidget* GetCharacterAttributesWidget();

	void SetHighlightInteractableVisability(bool bIsVisible);
	void SetHighlightInteractableActionText(FName KeyName);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names")
	FName ReticleWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names")
	FName AmmoWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names")
	FName CharacterAttributesName;

	UPROPERTY(meta = (BindWidget))
	UHighlightInteractableWidget* InteractableKey;
};
