#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SetMeleeRegEnabled.generated.h"

/**
 * 
 */
UCLASS()
class ACIDHOUSE_API UAnimNotify_SetMeleeRegEnabled : public UAnimNotify
{
	GENERATED_BODY()
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee weapon")
	bool bIsHitRegistrationEnabled;
};
