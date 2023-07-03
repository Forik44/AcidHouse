#include "Actors/Equipment/Throwables/ThrowableItem.h"
#include "Characters/AHBaseCharacter.h"
#include "Actors/Projectiles/AHProjectile.h"

void AThrowableItem::Throw()
{
	checkf(GetOwner()->IsA<AAHBaseCharacter>(), TEXT("AThrowableItem::Throw() only character can be an owner of throwable weapon"));
	AAHBaseCharacter* CharacterOwner = StaticCast<AAHBaseCharacter*>(GetOwner());

	APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
	if (!IsValid(Controller))
	{
		return;
	}

	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;
	Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);

	FVector ViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);

	FVector SpawnLocation = PlayerViewPoint + ViewDirection*100.0f;
	AAHProjectile* Projectile = GetWorld()->SpawnActor<AAHProjectile>(ProjectileClass, SpawnLocation, FRotator::ZeroRotator);
	if (IsValid(Projectile))
	{
		Projectile->SetOwner(GetOwner());
		Projectile->LaunchProjectile(ViewDirection);
	}
}
