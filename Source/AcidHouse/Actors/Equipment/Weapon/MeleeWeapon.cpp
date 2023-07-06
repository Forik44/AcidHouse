#include "Actors/Equipment/Weapon/MeleeWeapon.h"
#include "Characters/AHBaseCharacter.h"
#include "Components/Weapon/MeleeHitRegistrator.h"
#include "GameFramework/Controller.h"


AMeleeWeapon::AMeleeWeapon()
{
	EquippedSocketName = SocketCharacterWeapon;
}

void AMeleeWeapon::StartAttack(EMeleeAttackTypes AttackType)
{
	AAHBaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!IsValid(CharacterOwner))
	{
		return;
	}

	HitActors.Empty();

	CurrentAttack = Attacks.Find(AttackType);
	if (CurrentAttack != nullptr && IsValid(CurrentAttack->AttackMontage))
	{
		UAnimInstance* CharacterAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
		if (IsValid(CharacterAnimInstance))
		{
			float Duration = CharacterAnimInstance->Montage_Play(CurrentAttack->AttackMontage, 1.0f, EMontagePlayReturnType::Duration);
			GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AMeleeWeapon::OnAttackTimerElapsed, Duration, false);
		}
		else
		{
			OnAttackTimerElapsed();
		}
	}
}

void AMeleeWeapon::SetIsHitRegistratorEnabled(bool bIsRegistrationEnabled)
{
	HitActors.Empty();
	for (UMeleeHitRegistrator* HitRegistrator : HitRegistartors)
	{
		HitRegistrator->SetIsHitRegistarationEnabled(bIsRegistrationEnabled);
	}
}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	GetComponents<UMeleeHitRegistrator>(HitRegistartors);

	for (UMeleeHitRegistrator* HitRegistrator : HitRegistartors)
	{
		HitRegistrator->OnMeleeHitRegistred.AddUFunction(this, FName("ProcessHit"));
	}
}

void AMeleeWeapon::ProcessHit(const FHitResult& HitResult, FVector& HitDirection)
{
	if (CurrentAttack == nullptr)
	{
		return;
	}

	AActor* HitActor = HitResult.GetActor();
	if (!IsValid(HitActor))
	{
		return;
	}

	if (HitActors.Contains(HitActor))
	{
		return;
	}

	FPointDamageEvent DamageEvent;
	DamageEvent.HitInfo = HitResult;
	DamageEvent.ShotDirection = HitDirection;
	DamageEvent.DamageTypeClass = CurrentAttack->DamageType;

	AAHBaseCharacter* CharacterOwner = GetCharacterOwner();
	AController* Controller = IsValid(CharacterOwner) ? CharacterOwner->GetController<AController>() : nullptr;
	HitActor->TakeDamage(CurrentAttack->Damage, DamageEvent, Controller, GetOwner());

	HitActors.Add(HitActor);
}

void AMeleeWeapon::OnAttackTimerElapsed()
{
	CurrentAttack = nullptr;
	SetIsHitRegistratorEnabled(false);
}
