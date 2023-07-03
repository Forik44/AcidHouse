#include "Components/Weapon/WeaponBarellComponent.h"
#include "AcidHouseTypes.h"
#include "DrawDebugHelpers.h"
#include "Subsystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/DecalComponent.h"

void UWeaponBarellComponent::Shot(FVector ShotStart, FVector ShotDirection, AController* Controller, float SpreadAngle)
{
	for (int i = 0; i < BulletsPerShot; i++)
	{
		ShotDirection += GetBulletSpreadOffset(FMath::RandRange(0.0f, SpreadAngle), ShotDirection.ToOrientationRotator());

		FVector MuzzleLocation = GetComponentLocation();
		FVector ShotEnd = ShotStart + FiringRange * ShotDirection;

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashFX, MuzzleLocation, GetComponentRotation());

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
				if (IsValid(FalloffDiagram))
				{
					HitActor->TakeDamage(DamageAmount * FalloffDiagram->GetFloatValue((MuzzleLocation - ShotEnd).Size() * 0.01), FDamageEvent{}, Controller, GetOwner());
				}
				FPointDamageEvent DamageEvent;
				DamageEvent.HitInfo = ShotResult;
				DamageEvent.ShotDirection = ShotDirection;
				DamageEvent.DamageTypeClass = DamageTypeClass;
				HitActor->TakeDamage(DamageAmount, DamageEvent, Controller, GetOwner());
			}
		}

		UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultDecalInfo.DecalMaterial, DefaultDecalInfo.DecalSize, ShotResult.ImpactPoint, ShotResult.ImpactNormal.ToOrientationRotator());
		if (IsValid(DecalComponent))
		{
			float DefaultDecalFadeScreenSize = 0.0001f;
			DecalComponent->SetFadeScreenSize(DefaultDecalFadeScreenSize);
			DecalComponent->SetFadeOut(DefaultDecalInfo.DecalLifetime, DefaultDecalInfo.DecalFadeOutTime);
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
