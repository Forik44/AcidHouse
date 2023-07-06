#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "MeleeHitRegistrator.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FMeleeHitRegistred, const FHitResult&, const FVector&);

UCLASS(meta = (BlueprintSpawnableComponent))
class ACIDHOUSE_API UMeleeHitRegistrator : public USphereComponent
{
	GENERATED_BODY()
	
public:
	UMeleeHitRegistrator();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ProcessHitRegistration();

	void SetIsHitRegistarationEnabled(bool bIsEnabled_In);

	FMeleeHitRegistred OnMeleeHitRegistred;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee hit registration")
	bool bIsHitRegistrationEnabled = false;

private:
	FVector PreviousComponentLocation = FVector::ZeroVector;
};
