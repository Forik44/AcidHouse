#include "Components/Weapon/WeaponBarellComponent.h"
#include "AcidHouseTypes.h"
#include "DrawDebugHelpers.h"
#include "Subsystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/DecalComponent.h"
#include "Actors/Projectiles/AHProjectile.h"
#include "Net/UnrealNetwork.h"

UWeaponBarellComponent::UWeaponBarellComponent()
{
	SetIsReplicatedByDefault(true);
}

void UWeaponBarellComponent::Shot(FVector ShotStart, FVector ShotDirection, float SpreadAngle)
{
	TArray<FShotInfo> ShotsInfo;
	for (int i = 0; i < BulletsPerShot; i++)
	{
		ShotDirection += GetBulletSpreadOffset(FMath::RandRange(0.0f, SpreadAngle), ShotDirection.ToOrientationRotator());
		FVector ShotEnd = ShotStart + FiringRange * ShotDirection;

		ShotsInfo.Emplace(ShotStart, ShotDirection);
	}

	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_Shot(ShotsInfo);
	}
	ShotInternal(ShotsInfo);
}

void UWeaponBarellComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams RepParams;
	RepParams.Condition = COND_SimulatedOnly;
	RepParams.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME_WITH_PARAMS(UWeaponBarellComponent, LastShotsInfo, RepParams);
}

bool UWeaponBarellComponent::HitScan(FVector ShotStart, OUT FVector& ShotEnd, FVector ShotDirection)
{
	FHitResult ShotResult;
	bool bHasHit = GetWorld()->LineTraceSingleByChannel(ShotResult, ShotStart, ShotEnd, ECC_Bullet);
	if (bHasHit)
	{
		ProcessHit(ShotResult, ShotDirection);
	}		
	return bHasHit;
}

void UWeaponBarellComponent::LauchProjectile(const FVector& LauchStart, const FVector& LaunchDirection)
{
	AAHProjectile* Projectile = GetWorld()->SpawnActor<AAHProjectile>(ProjectileClass, LauchStart, LaunchDirection.ToOrientationRotator());
	if (IsValid(Projectile))
	{
		Projectile->SetOwner(GetOwningPawn());
		Projectile->OnProjectileHit.AddDynamic(this, &UWeaponBarellComponent::ProcessHit);
		Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
	}
}

APawn* UWeaponBarellComponent::GetOwningPawn() const
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (IsValid(PawnOwner))
	{
		PawnOwner = Cast<APawn>(GetOwner()->GetOwner());
	}
	return PawnOwner;
}

AController* UWeaponBarellComponent::GetController() const
{
	APawn* PawnOwner = GetOwningPawn();
	return IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr;
}

void UWeaponBarellComponent::Server_Shot_Implementation(const TArray<FShotInfo>& ShotsInfo)
{
	ShotInternal(ShotsInfo);
}

void UWeaponBarellComponent::OnRep_LastShotsInfo()
{
	ShotInternal(LastShotsInfo);
}

void UWeaponBarellComponent::ProcessHit(const FHitResult& HitResult, const FVector& Direction)
{
	AActor* HitActor = HitResult.GetActor();
	AController* Controller = GetController();
	if (IsValid(HitActor))
	{
		if (IsValid(FalloffDiagram))
		{
			FVector ShotEnd = HitResult.ImpactPoint;
			FVector MuzzleLocation = GetComponentLocation();
			HitActor->TakeDamage(DamageAmount * FalloffDiagram->GetFloatValue((MuzzleLocation - ShotEnd).Size() * 0.01), FDamageEvent{}, Controller, GetOwner());
		}
		FPointDamageEvent DamageEvent;
		DamageEvent.HitInfo = HitResult;
		DamageEvent.ShotDirection = Direction;
		DamageEvent.DamageTypeClass = DamageTypeClass;
		HitActor->TakeDamage(DamageAmount, DamageEvent, Controller, GetOwner());
	}

	UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultDecalInfo.DecalMaterial, DefaultDecalInfo.DecalSize, HitResult.ImpactPoint, HitResult.ImpactNormal.ToOrientationRotator());
	if (IsValid(DecalComponent))
	{
		float DefaultDecalFadeScreenSize = 0.0001f;
		DecalComponent->SetFadeScreenSize(DefaultDecalFadeScreenSize);
		DecalComponent->SetFadeOut(DefaultDecalInfo.DecalLifetime, DefaultDecalInfo.DecalFadeOutTime);
	}
}

void UWeaponBarellComponent::ShotInternal(const TArray<FShotInfo>& ShotsInfo)
{
	if (GetOwner()->HasAuthority())
	{
		LastShotsInfo = ShotsInfo;
	}

	FVector MuzzleLocation = GetComponentLocation();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashFX, MuzzleLocation, GetComponentRotation());
	for (const FShotInfo& ShotInfo : ShotsInfo)
	{
		FVector ShotStart = ShotInfo.GetLocation();
		FVector ShotDirection = ShotInfo.GetDirection();

		FVector ShotEnd = ShotStart + FiringRange * ShotDirection;

#if ENABLE_DRAW_DEBUG 
		UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
		bool bIsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryRangeWeapon);
#else
		bool bIsDebugEnabled = false;
#endif

		switch (HitRegistrationType)
		{
		case EHitRegistrationType::HitScan:
		{
			bool bHasHit = HitScan(ShotStart, ShotEnd, ShotDirection);
			if (bIsDebugEnabled && bHasHit)
			{
				DrawDebugSphere(GetWorld(), ShotEnd, 10.0f, 24, FColor::Red, false, 1.0f);
			}
			break;
		}
		case EHitRegistrationType::Projectile:
		{
			LauchProjectile(ShotStart, ShotDirection);
			break;
		}
		default:
			break;
		}

		UNiagaraComponent* TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, MuzzleLocation, GetComponentRotation());
		TraceFXComponent->SetVectorParameter(FXParamTraceEnd, ShotEnd);

		if (bIsDebugEnabled)
		{
			DrawDebugLine(GetWorld(), MuzzleLocation, ShotEnd, FColor::Red, false, 1.0f, 0, 3.0f);
		}
	}
}

FVector UWeaponBarellComponent::GetBulletSpreadOffset(float Angle, FRotator ShotRotation)
{
	float SpreadSize = FMath::Tan(Angle);
	float RotationAngle = FMath::RandRange(0.0f, 2 * PI);

	float SpreadY = FMath::Cos(RotationAngle);
	float SpreadZ = FMath::Sin(RotationAngle);

	FVector Result = (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ
		+ ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;

	return Result;
}
