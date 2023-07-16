#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AHProjectile.generated.h"

class AAHProjectile;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnProjectileHit, AAHProjectile*, Projectile, const FHitResult&, Hit, const FVector&, Direction);

UCLASS()
class ACIDHOUSE_API AAHProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AAHProjectile();

	UPROPERTY(BlueprintAssignable)
	FOnProjectileHit OnProjectileHit;

	UFUNCTION(BlueprintCallable)
	void LaunchProjectile(FVector Direction);

	UFUNCTION(BlueprintNativeEvent)
	void SetProjectileActive(bool bIsProjectileActive);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	virtual void OnProjectileLaunched();

private:
	UFUNCTION()
	void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
