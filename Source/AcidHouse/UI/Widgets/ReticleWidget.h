#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AcidHouseTypes.h"
#include "ReticleWidget.generated.h"

class AEquipableItem;
UCLASS()
class ACIDHOUSE_API UReticleWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnAimingStateChanged(bool bIsAiming);

	UFUNCTION(BlueprintNativeEvent)
	void OnEquippedItemChanged(const AEquipableItem* EquippedItem);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reticle")
	EReticleType CurrentReticle;

private:
	TWeakObjectPtr<const AEquipableItem> CurrentEquippedItem;

	void SetupCurrentReticle();
};
