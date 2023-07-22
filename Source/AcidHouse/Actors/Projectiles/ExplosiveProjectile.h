#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectiles/AHProjectile.h"
#include "ExplosiveProjectile.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnExplosiveProjectileEndLife, AExplosiveProjectile*)

class UExplosionComponent;
UCLASS()
class ACIDHOUSE_API AExplosiveProjectile : public AAHProjectile
{
	GENERATED_BODY()
	
public:
	AExplosiveProjectile();

	FOnExplosiveProjectileEndLife OnExplosiveProjectileEndLife;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UExplosionComponent* ExplosionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float DetonationTime = 3.8f;

	virtual void OnProjectileLaunched() override;

private:
	void OnDetonationTimerElapsed();

	AController* GetController();

	FTimerHandle DetonationTimer;
};
