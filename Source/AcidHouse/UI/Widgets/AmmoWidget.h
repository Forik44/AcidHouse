#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoWidget.generated.h"

UCLASS()
class ACIDHOUSE_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo")
	int32 Ammo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo")
	int32 TotalAmmo;

private:
	UFUNCTION()
	void UpdateAmmoCount(int32 NewAmmo, int32 NewTotalAmmo);
};
