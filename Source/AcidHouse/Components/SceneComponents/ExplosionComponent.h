#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ExplosionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExplosion);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACIDHOUSE_API UExplosionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintCallable)
	void Explode(AController* Controller);

	UPROPERTY(BlueprintAssignable)
	FOnExplosion OnExplosion;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion | Damage", meta = (ClampMin = 0, UIMin = 0))
	float MaxDamage = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion | Damage", meta = (ClampMin = 0, UIMin = 0))
	float MinDamage = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion | Damage", meta = (ClampMin = 0, UIMin = 0))
	float DamageFalloff = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion | Damage")
	TSubclassOf<class UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion | Radius", meta = (ClampMin = 0, UIMin = 0))
	float InnerRadius = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion | Radius", meta = (ClampMin = 0, UIMin = 0))
	float OuterRadius = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion | VFX")
	UParticleSystem* ExplosionVFX;
};
