#include "Components/Weapon/WeaponBarellComponent.h"
#include "AcidHouseTypes.h"
#include "DrawDebugHelpers.h"
#include "Subsystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UWeaponBarellComponent::Shot(FVector ShotStart, FVector ShotDirection, AController* Controller)
{
	FVector MuzzleLocation = GetComponentLocation();
	FVector ShotEnd = ShotStart + FiringRange * ShotDirection;

#if ENABLE_DRAW_DEBUG 
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryRangeWeapon);
#else
	bool bIsDebugEnabled = false;
#endif

	FHitResult ShotResult;
	if (GetWorld()->LineTraceSingleByChannel(ShotResult, ShotStart, ShotEnd, ECC_Bullet))
	{
		ShotEnd = ShotResult.ImpactPoint;
		if (bIsDebugEnabled)
		{
			DrawDebugSphere(GetWorld(), ShotEnd, 10.0f, 24, FColor::Red, false, 1.0f);
		}

		AActor* HitActor = ShotResult.GetActor();
		if (IsValid(HitActor))
		{
			HitActor->TakeDamage(DamageAmount, FDamageEvent{}, Controller, GetOwner());
		}
	}
	if (bIsDebugEnabled)
	{
		DrawDebugLine(GetWorld(), MuzzleLocation, ShotEnd, FColor::Red, false, 1.0f, 0, 3.0f);
	}
}
