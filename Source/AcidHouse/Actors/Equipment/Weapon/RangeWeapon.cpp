// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Equipment/Weapon/RangeWeapon.h"
#include "Components/Weapon/WeaponBarellComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AcidHouseTypes.h"
#include "Characters/AHBaseCharacter.h"

ARangeWeapon::ARangeWeapon()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarell"));
	WeaponBarell->SetupAttachment(WeaponMesh, SocketWeaponMuzzle);

}

void ARangeWeapon::Fire()
{
	checkf(GetOwner()->IsA<AAHBaseCharacter>(), TEXT("ARangeWeapon::Fire() only character can be an owner of range weapon"));
	AAHBaseCharacter* CharacterOwner = StaticCast<AAHBaseCharacter*>(GetOwner());

	CharacterOwner->PlayAnimMontage(CharacterFireMontage);
	PlayAnimMontage(WeaponFireMontage);

	APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
	if (!IsValid(Controller))
	{
		return;
	}

	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;
	Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);

	FVector ViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);

	WeaponBarell->Shot(PlayerViewPoint , ViewDirection, Controller);
}

FTransform ARangeWeapon::GetForeGripTransform() const
{
	return WeaponMesh->GetSocketTransform(SocketWeaponForeGrip);
}

float ARangeWeapon::PlayAnimMontage(UAnimMontage* AnimMontage)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	float Result = 0.0f;
	if (IsValid(WeaponAnimInstance))
	{
		Result = WeaponAnimInstance->Montage_Play(AnimMontage);
	}
	return Result;
}
