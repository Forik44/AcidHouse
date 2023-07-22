#include "Actors/Equipment/Throwables/ThrowableItem.h"
#include "Characters/AHBaseCharacter.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Net/UnrealNetwork.h"
#include "Actors/Projectiles/ExplosiveProjectile.h"

AThrowableItem::AThrowableItem()
{
	SetReplicates(true);
}

void AThrowableItem::Initialize()
{
	if (!GetOwner())
	{
		return;
	}

	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	if (!IsValid(ProjectileClass))
	{
		return;
	}

	ProjectilePool.Reserve(ProjectilePoolSize);

	for (int32 i = 0; i < ProjectilePoolSize; ++i)
	{
		AAHProjectile* Projectile = GetWorld()->SpawnActor<AAHProjectile>(ProjectileClass, ProjectilePoolLocation, FRotator::ZeroRotator);
		if (IsValid(Projectile))
		{
			Projectile->SetOwner(GetOwner());
			Projectile->SetProjectileActive(false);
			ProjectilePool.Add(Projectile);
		}
	}
}

void AThrowableItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams RepParams;
	RepParams.Condition = COND_SimulatedOnly;
	RepParams.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME_WITH_PARAMS(AThrowableItem, LastThrowInfo, RepParams);
	DOREPLIFETIME(AThrowableItem, ProjectilePool);
	DOREPLIFETIME(AThrowableItem, CurrentProjectileIndex);
}

void AThrowableItem::Throw()
{
	AAHBaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!IsValid(CharacterOwner))
	{
		return;
	}

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

	FThrowInfo ThrowInfo(SpawnLocation, LaunchDirection);

	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_Throw(ThrowInfo);
	}
	ThrowIternal(ThrowInfo);

	int32 Ammo = CharacterOwner->GetCharacterEquipmentComponent_Mutable()->GetAmmoCurrentThrowableItem();
	CharacterOwner->GetCharacterEquipmentComponent_Mutable()->SetAmmoCurrentThrowableItem(Ammo - 1);
}

void AThrowableItem::ThrowIternal(const FThrowInfo& ThrowInfo)
{
	if (GetOwner()->HasAuthority())
	{
		LastThrowInfo = ThrowInfo;
	}

	AAHProjectile* Projectile = ProjectilePool[CurrentProjectileIndex];

	if (!IsValid(Projectile))
	{
		return;
	}

	FVector SpawnLocation = ThrowInfo.GetLocation();
	FVector LaunchDirection = ThrowInfo.GetDirection();

	Projectile->SetActorLocation(SpawnLocation);
	Projectile->SetActorRotation(LaunchDirection.ToOrientationRotator());
	Projectile->SetProjectileActive(true);

	AExplosiveProjectile* ExplosiveProjectile = Cast<AExplosiveProjectile>(Projectile);
	if (IsValid(ExplosiveProjectile))
	{
		ExplosiveProjectile->OnExplosiveProjectileEndLife.AddUFunction(this, FName("OnExplosiveProjectileEndLife"));
	}

	Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
	
	++CurrentProjectileIndex;
	if (CurrentProjectileIndex == ProjectilePool.Num())
	{
		CurrentProjectileIndex = 0;
	}
}

void AThrowableItem::Server_Throw_Implementation(const FThrowInfo& ThrowInfo)
{
	ThrowIternal(ThrowInfo);
}

void AThrowableItem::OnRep_LastThrowInfo(const FThrowInfo& ThrowInfo_Old)
{
	ThrowIternal(LastThrowInfo);
}

void AThrowableItem::OnExplosiveProjectileEndLife(AExplosiveProjectile* Projectile)
{
	Projectile->SetProjectileActive(false);
	Projectile->SetActorLocation(ProjectilePoolLocation);
	Projectile->SetActorRotation(FRotator::ZeroRotator);
	Projectile->OnExplosiveProjectileEndLife.RemoveAll(this);
}
