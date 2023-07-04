#include "Actors/Equipment/Throwables/ThrowableItem.h"
#include "Characters/AHBaseCharacter.h"
#include "Actors/Projectiles/AHProjectile.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"

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

	FTransform PlayerViewTransform(PlayerViewRotation, PlayerViewPoint);

	FVector ViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);
	FVector ViewUpVector = PlayerViewRotation.RotateVector(FVector::UpVector);

	FVector LaunchDirection = ViewDirection + FMath::Tan(FMath::DegreesToRadians(ThrowAngle)) * ViewUpVector;

	FVector ThrowableSocketLocation = CharacterOwner->GetMesh()->GetSocketLocation(SocketCharacterThrowable);
	FVector SocketInViewSpace = PlayerViewTransform.InverseTransformPosition(ThrowableSocketLocation);

	FVector SpawnLocation = PlayerViewPoint + ViewDirection * SocketInViewSpace.X + CharacterOwner->GetActorRightVector() * SocketInViewSpace.Y;
	AAHProjectile* Projectile = GetWorld()->SpawnActor<AAHProjectile>(ProjectileClass, SpawnLocation, ViewDirection.ToOrientationRotator());
	if (IsValid(Projectile))
	{
		Projectile->SetOwner(GetOwner());
		Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());

		int32 Ammo = CharacterOwner->GetCharacterEquipmentComponent_Mutable()->GetAmmoCurrentThrowableItem();
		CharacterOwner->GetCharacterEquipmentComponent_Mutable()->SetAmmoCurrentThrowableItem(Ammo - 1);
	}
}
