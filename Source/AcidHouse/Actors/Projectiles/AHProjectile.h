#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AHProjectile.generated.h"

UCLASS()
class ACIDHOUSE_API AAHProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AAHProjectile();

	UFUNCTION(BlueprintCallable)
	void LaunchProjectile(FVector Direction);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	virtual void OnProjectileLaunched();

};
