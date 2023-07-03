// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ReloadAmmo.generated.h"

/**
 * 
 */
UCLASS()
class ACIDHOUSE_API UAnimNotify_ReloadAmmo : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (ClampMin = 1, UIMin = 1))
	int32 NubmberOfAmmo = 1;

	
};
