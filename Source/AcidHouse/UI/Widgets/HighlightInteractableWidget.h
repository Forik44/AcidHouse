#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HighlightInteractableWidget.generated.h"

class UTextBlock;
UCLASS()
class ACIDHOUSE_API UHighlightInteractableWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetActionText(FName KeyName);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ActionText;
};
