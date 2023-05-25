#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeathEventSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOutOfStaminaEventSignature, bool, bIsOutOfStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOutOfOxygenEventSignature, bool, bIsOutOfOxygen);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACIDHOUSE_API UCharacterAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnDeathEventSignature OnDeathEvent;
	FOnOutOfStaminaEventSignature OnOutOfStaminaEvent;
	FOnOutOfOxygenEventSignature OnOutOfOxygenEvent;

	bool IsAlive() { return Health > 0.0f; }

	float GetHealthPersent() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetCurrentStamina() const { return Stamina; }
	FORCEINLINE float GetOutOfOxygenDamageRate() const { return OutOfOxygenDamageRate;  }
	FORCEINLINE float GetOutOfOxygenDamageAmount() const { return OutOfOxygenDamageAmount; }
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (UIMin = 0.0f))
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = 0, UIMin = 0))
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = 0, UIMin = 0))
	float StaminaRestoreVelocity = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = 0, UIMin = 0))
	float SprintStaminaConsumptionVelocity = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oxygen", meta = (ClampMin = 0, UIMin = 0))
	float MaxOxygen = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oxygen", meta = (ClampMin = 0, UIMin = 0))
	float OxygenRestoreVelocity = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oxygen", meta = (ClampMin = 0, UIMin = 0))
	float SwimOxygenConsumptionVelocity = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oxygen", meta = (ClampMin = 0, UIMin = 0))
	float OutOfOxygenDamageRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Oxygen", meta = (ClampMin = 0, UIMin = 0))
	float OutOfOxygenDamageAmount = 10.0f;

private:
	float Health = 0.0f;
	float Stamina = 0.0f;
	float Oxygen = 0.0f;

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	void DebugDrawAttributes();
#endif

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void UpdateStaminaValue(float DeltaTime);
	void UpdateOxygenValue(float DeltaTime);


	TWeakObjectPtr<class AAHBaseCharacter> CashedBaseCharacterOwner; 
		
};
