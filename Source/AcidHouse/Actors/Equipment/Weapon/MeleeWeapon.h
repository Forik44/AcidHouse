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

UCLASS(Blueprintable)
class ACIDHOUSE_API AMeleeWeapon : public AEquipableItem
{
	GENERATED_BODY()

public:
	AMeleeWeapon();

	void StartAttack(EMeleeAttackTypes AttackType);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee Attack")
	TMap<EMeleeAttackTypes, FMeleeAttackDescription> Attacks;

private:
	FTimerHandle AttackTimer;
	FMeleeAttackDescription* CurrentAttack;

	void OnAttackTimerElapsed();
};
