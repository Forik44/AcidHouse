#include "AI/Characters/Turret.h"
#include "Components/Weapon/WeaponBarellComponent.h"
#include "AIController.h"
#include "GenericTeamAgentInterface.h"
#include "Perception/AISense_Damage.h"
#include "Components/SceneComponents/ExplosionComponent.h"
#include "Net/UnrealNetwork.h"

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

	ExplosionComponent = CreateDefaultSubobject<UExplosionComponent>(TEXT("ExplosionComponent"));
	ExplosionComponent->SetupAttachment(TurretRoot);

	SetReplicates(true);
	NetUpdateFrequency = 2.0f;
	MinNetUpdateFrequency = 2.0f;
}

void ATurret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATurret, CurrentTarget);
	DOREPLIFETIME(ATurret, Health);
}

void ATurret::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AAIController* AIController = Cast<AAIController>(NewController);
	if (IsValid(AIController))
	{
		FGenericTeamId TeamID((uint8)Team);
		AIController->SetGenericTeamId(TeamID);
	}
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsAlive)
	{
		return;
	}

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

void ATurret::OnCurrentTargetSet()
{
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

float ATurret::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!bIsAlive)
	{
		return 0;
	}
	UAISense_Damage::ReportDamageEvent(GetWorld(), this, DamageCauser, Damage, DamageCauser->GetActorLocation(), GetActorLocation());
	SetHealth(Health - Damage);

	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void ATurret::SetHealth(float NewHealth)
{
	Health = NewHealth;
	if (Health <= 0)
	{
		OnDeath();
	}
}

void ATurret::BeginPlay()
{
	Super::BeginPlay();
	
	Health = MaxHealth;
}

void ATurret::OnDeath_Implementation()
{
	bIsAlive = false;
	GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
	ExplosionComponent->Explode(GetController());
}

void ATurret::OnRep_CurrentTarget()
{
	OnCurrentTargetSet();
}

void ATurret::OnRep_Health()
{
	SetHealth(Health);
}

float ATurret::GetFireInterval() const
{
	return 60.f / RateOfFire;
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
	FVector BaseLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBaseComponent->GetComponentLocation()).GetSafeNormal2D();
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
	bool bIsStateChanged = NewState != CurrentTurretState;
	CurrentTurretState = NewState;

	if (!bIsStateChanged || !bIsAlive)
	{
		return;
	}

	switch (CurrentTurretState)
	{
		case ETurretState::Searching:
		{
			GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
			break;
		}
		case ETurretState::Firing:
		{
			GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ATurret::MakeShot, GetFireInterval(), true, FireDelayTime);
			break;
		}
		default:
			break;
	}
}

void ATurret::MakeShot()
{
	if (!bIsAlive)
	{
		return;
	}

	FVector ShotLocation = WeaponBarell->GetComponentLocation();
	FVector ShotDirection = WeaponBarell->GetComponentRotation().RotateVector(FVector::ForwardVector);
	float SpreadAngle = FMath::DegreesToRadians(BulletSpreadAngle);
	WeaponBarell->Shot(ShotLocation, ShotDirection, SpreadAngle);
}

