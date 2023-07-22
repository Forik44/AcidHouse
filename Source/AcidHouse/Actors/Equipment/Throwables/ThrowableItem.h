#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "ThrowableItem.generated.h"

USTRUCT(BlueprintType)
struct FThrowInfo
{
	GENERATED_BODY()

	FThrowInfo()
		: SpawnLocation_Mul_10(FVector::ZeroVector),
		Direction(FVector::ZeroVector){};

	FThrowInfo(FVector Location, FVector Direction)
		: SpawnLocation_Mul_10(Location * 10.0f), 
		Direction(Direction){};

	UPROPERTY()
	FVector SpawnLocation_Mul_10;

	UPROPERTY()
	FVector Direction;

	FVector GetLocation() const { return SpawnLocation_Mul_10 * 0.1f; }
	FVector GetDirection() const { return Direction; }
};

UCLASS(Blueprintable)
class ACIDHOUSE_API AThrowableItem : public AEquipableItem
{
	GENERATED_BODY()
	
public:
	AThrowableItem();

	virtual void Initialize();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Throw();

	EAmunitionType GetAmmoType() { return AmmoType; }
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables")
	TSubclassOf<class AAHProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Throwables", meta = (UIMin = -90.0f, UIMax = 90.0f, ClampMin = -90.0f, ClampMax = 90.0f))
	float ThrowAngle = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables")
	EAmunitionType AmmoType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables", meta = (UIMin = 10, ClampMin = 10))
	int32 ProjectilePoolSize = 10;

private:
	UPROPERTY(ReplicatedUsing = OnRep_LastThrowInfo)
	FThrowInfo LastThrowInfo;

	UPROPERTY(Replicated)
	TArray<AAHProjectile*> ProjectilePool;

	UPROPERTY(Replicated)
	int32 CurrentProjectileIndex;

	const FVector ProjectilePoolLocation = FVector(0.0f, 0.0f, -100.0f);

	void ThrowIternal(const FThrowInfo& ThrowInfo);

	UFUNCTION(Server, Reliable)
	void Server_Throw(const FThrowInfo& ThrowInfo);

	UFUNCTION()
	void OnRep_LastThrowInfo(const FThrowInfo& ThrowInfo_Old);

	UFUNCTION()
	void OnExplosiveProjectileEndLife(class AExplosiveProjectile* Projectile);

};
