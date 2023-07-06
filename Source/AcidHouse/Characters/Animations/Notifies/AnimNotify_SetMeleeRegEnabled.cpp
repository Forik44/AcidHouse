#include "Characters/Animations/Notifies/AnimNotify_SetMeleeRegEnabled.h"
#include "../../AHBaseCharacter.h"
#include "Actors/Equipment/Weapon/MeleeWeapon.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"

void UAnimNotify_SetMeleeRegEnabled::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	AAHBaseCharacter* CharacterOwner = Cast<AAHBaseCharacter>(MeshComp->GetOwner());
	if (!IsValid(CharacterOwner))
	{
		return;
	}

	AMeleeWeapon* MeleeWeapon = CharacterOwner->GetCharacterEquipmentComponent()->GetCurrentMeleeWeapon();
	if (IsValid(MeleeWeapon))
	{
		MeleeWeapon->SetIsHitRegistratorEnabled(bIsHitRegistrationEnabled);
	}

}
