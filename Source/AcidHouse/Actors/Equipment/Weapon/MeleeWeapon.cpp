#include "Actors/Equipment/Weapon/MeleeWeapon.h"
#include "Characters/AHBaseCharacter.h"

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

void AMeleeWeapon::OnAttackTimerElapsed()
{
	CurrentAttack = nullptr;
}
