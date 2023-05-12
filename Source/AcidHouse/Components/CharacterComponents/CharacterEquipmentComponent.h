// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterEquipmentComponent.generated.h"

class ARangeWeapon;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACIDHOUSE_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSubclassOf<ARangeWeapon> SideArmClass;

private:
	ARangeWeapon* CurrentEquipmentItem;
	TWeakObjectPtr<class AAHBaseCharacter> CachedBaseCharacter;

	void CreateLoadout();

};
