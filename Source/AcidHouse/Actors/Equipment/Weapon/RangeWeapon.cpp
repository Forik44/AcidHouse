// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Equipment/Weapon/RangeWeapon.h"
#include "Components/Weapon/WeaponBarellComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AcidHouseTypes.h"

ARangeWeapon::ARangeWeapon()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarell"));
	WeaponBarell->SetupAttachment(WeaponMesh, SocketWeaponMuzzle);

}
