#include "AI/Characters/Turret.h"
#include "Components/Weapon/WeaponBarellComponent.h"

ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* TurretRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TurretRoot"));
	SetRootComponent(TurretRoot);

	TurretBaseComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TurretBase"));
	TurretBaseComponent->SetupAttachment(TurretRoot);

	TurretBarellComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TurretBarell"));
	TurretBarellComponent->SetupAttachment(TurretBaseComponent);

	WeaponBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarrel"));
	WeaponBarell->SetupAttachment(TurretBarellComponent);
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentTurretState)
	{
		case ETurretState::Searching:
		{
			SearchingMovement(DeltaTime);
			break;
		}
		case ETurretState::Firing:
		{
			TrackingMovement(DeltaTime);
			break;
		}
		default:
			break;
	}
}

void ATurret::SetCurrentTarget(AActor* NewTarget)
{
	CurrentTarget = NewTarget;
	ETurretState NewState = IsValid(CurrentTarget) ? ETurretState::Firing : ETurretState::Searching;
	SetCurrentTurretState(NewState);
}

FVector ATurret::GetPawnViewLocation() const
{
	return WeaponBarell->GetComponentLocation();
}

FRotator ATurret::GetViewRotation() const
{
	return WeaponBarell->GetComponentRotation();
}

void ATurret::SearchingMovement(float DeltaTime)
{
	FRotator TurretBaseRotation = TurretBaseComponent->GetRelativeRotation();
	TurretBaseRotation.Yaw += DeltaTime * BaseSearchingRotationRate;
	TurretBaseComponent->SetRelativeRotation(TurretBaseRotation);

	FRotator TurretBarrelRotation = TurretBarellComponent->GetRelativeRotation(); 
	TurretBarrelRotation.Pitch = FMath::FInterpTo(TurretBarrelRotation.Pitch, 0.0f, DeltaTime, BarrelPitchRotationRate);
	TurretBarellComponent->SetRelativeRotation(TurretBarrelRotation);
}

void ATurret::TrackingMovement(float DeltaTime)
{
	FVector BaseLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBaseComponent->GetComponentLocation()).GetSafeNormal();
	FQuat LookAtQuat = BaseLookAtDirection.ToOrientationQuat();
	FQuat TargetQuat = FMath::QInterpTo(TurretBaseComponent->GetComponentQuat(), LookAtQuat, DeltaTime, BaseTrackingInterpSpeed);
	TurretBaseComponent->SetWorldRotation(TargetQuat);

	FVector BarrelLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBarellComponent->GetComponentLocation()).GetSafeNormal();
	float LookAtPitchAngle = BarrelLookAtDirection.ToOrientationRotator().Pitch;

	FRotator TurretBarrelRotation = TurretBarellComponent->GetRelativeRotation();
	TurretBarrelRotation.Pitch = FMath::FInterpTo(TurretBarrelRotation.Pitch, LookAtPitchAngle, DeltaTime, BarrelPitchRotationRate);
	TurretBarellComponent->SetRelativeRotation(TurretBarrelRotation);
}

void ATurret::SetCurrentTurretState(ETurretState NewState)
{
	CurrentTurretState = NewState;
}

