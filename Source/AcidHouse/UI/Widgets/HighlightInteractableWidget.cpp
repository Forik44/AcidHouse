#include "UI/Widgets/HighlightInteractableWidget.h"
#include "UMG/Public/Components/TextBlock.h"

void UHighlightInteractableWidget::SetActionText(FName KeyName)
{
	if (IsValid(ActionText))
	{
		ActionText->SetText(FText::FromName(KeyName));
	}
}
