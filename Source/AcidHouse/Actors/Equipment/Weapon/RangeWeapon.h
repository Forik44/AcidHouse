// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EquipableItem.h"
#include "RangeWeapon.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ACIDHOUSE_API ARangeWeapon : public AEquipableItem
{
	GENERATED_BODY()

public:
	ARangeWeapon();

	void Fire();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWeaponBarellComponent* WeaponBarell;


};
