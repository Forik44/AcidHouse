#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "AcidHouseTypes.h"
#include "MeleeWeapon.generated.h"

USTRUCT(BlueprintType)
struct FMeleeAttackDescription
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Attack")
	TSubclassOf<class UDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Attack", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float Damage = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Attack")
	class UAnimMontage* AttackMontage;
};

DECLARE_MULTICAST_DELEGATE(FOnAttackEnded);

class UMeleeHitRegistrator;
UCLASS(Blueprintable)
class ACIDHOUSE_API AMeleeWeapon : public AEquipableItem
{
	GENERATED_BODY()

public:
	AMeleeWeapon();

	FOnAttackEnded OnAttackEnded;

	void StartAttack(EMeleeAttackTypes AttackType);

	void SetIsHitRegistratorEnabled(bool bIsRegistrationEnabled);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee Attack")
	TMap<EMeleeAttackTypes, FMeleeAttackDescription> Attacks;

private:
	TArray<UMeleeHitRegistrator*> HitRegistartors;
	TSet<AActor*> HitActors;

	FTimerHandle AttackTimer;
	FMeleeAttackDescription* CurrentAttack;

	UFUNCTION()
	void ProcessHit(const FHitResult& HitResult, FVector& HitDirection);

	void OnAttackTimerElapsed();
};
