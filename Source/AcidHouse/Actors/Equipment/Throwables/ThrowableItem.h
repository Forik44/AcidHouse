#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "ThrowableItem.generated.h"

UCLASS(Blueprintable)
class ACIDHOUSE_API AThrowableItem : public AEquipableItem
{
	GENERATED_BODY()
	
public:
	void Throw();

	EAmunitionType GetAmmoType() { return AmmoType; }
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables")
	TSubclassOf<class AAHProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throwables", meta = (UIMin = -90.0f, UIMax = 90.0f, ClampMin = -90.0f, ClampMax = 90.0f))
	float ThrowAngle = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables")
	EAmunitionType AmmoType;
};
