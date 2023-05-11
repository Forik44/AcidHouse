#include "CharacterAttributeComponent.h"
#include "../../Characters/AHBaseCharacter.h"
#include "../../Subsystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "../../AcidHouseTypes.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "../MovementComponents/AHBaceCharacerMovementComponent.h"

UCharacterAttributeComponent::UCharacterAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCharacterAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(GetOwner()->IsA<AAHBaseCharacter>(), TEXT("UCharacterAttributeComponent::BeginPlay() Can be used only with AAHBaseCharacter"));
	CashedBaseCharacterOwner = StaticCast<AAHBaseCharacter*>(GetOwner());

	Health = MaxHealth;
	Stamina = MaxStamina;
	Oxygen = MaxOxygen;

	CashedBaseCharacterOwner->OnTakeAnyDamage.AddDynamic(this, &UCharacterAttributeComponent::OnTakeAnyDamage);
}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void UCharacterAttributeComponent::DebugDrawAttributes()
{
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	if (!DebugSubsystem->IsCategoryEnabled(DebugCategoryCharacterAttributes))
	{
		return;
	}

	FVector TextLocation = CashedBaseCharacterOwner->GetActorLocation() + (CashedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 5.0f) * FVector::UpVector;
	DrawDebugString(GetWorld(), TextLocation, FString::Printf(TEXT("Health: %.2f"), Health), nullptr, FColor::Red, 0.0f, true);
	DrawDebugString(GetWorld(), TextLocation + FVector(0, 0, 15), FString::Printf(TEXT("Stamina: %.2f"), Stamina), nullptr, FColor::Green, 0.0f, true);
	DrawDebugString(GetWorld(), TextLocation + FVector(0, 0, 30), FString::Printf(TEXT("Oxygen: %.2f"), Oxygen), nullptr, FColor::Blue, 0.0f, true);

}
#endif

void UCharacterAttributeComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsAlive())
	{
		return;
	}

	UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributeComponent::OnTakeAnyDamage %s recevied %.2f amount of damage from %s"), *CashedBaseCharacterOwner->GetName(), Damage, *DamageCauser->GetName());
	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);

	if (Health <= 0.0f)
	{
		UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributeComponent::OnTakeAnyDamage character %s is killed by an actor %s"), *CashedBaseCharacterOwner->GetName(), *DamageCauser->GetName());
		if (OnDeathEvent.IsBound())
		{
			OnDeathEvent.Broadcast();
		}
	}
}

void UCharacterAttributeComponent::UpdateStaminaValue(float DeltaTime)
{
	if (!CashedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsSprinting() && !CashedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsFastSwimming())
	{
		Stamina += StaminaRestoreVelocity * DeltaTime;
		Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);
		if (Stamina == MaxStamina)
		{
			if (OnOutOfStaminaEvent.IsBound())
			{
				OnOutOfStaminaEvent.Broadcast(false);
			}
		}
	}

	if (CashedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsSprinting())
	{
		Stamina -= SprintStaminaConsumptionVelocity * DeltaTime;
		Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);
		if (FMath::IsNearlyZero(Stamina, 1e-6f))
		{
			if (OnOutOfStaminaEvent.IsBound())
			{
				OnOutOfStaminaEvent.Broadcast(true);
			}
		}
	}

	if (CashedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsFastSwimming())
	{
		Stamina -= SprintStaminaConsumptionVelocity * DeltaTime;
		Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);
		if (FMath::IsNearlyZero(Stamina, 1e-6f))
		{
			if (OnOutOfStaminaEvent.IsBound())
			{
				OnOutOfStaminaEvent.Broadcast(true);
			}
		}
	}
}

void UCharacterAttributeComponent::UpdateOxygenValue(float DeltaTime)
{
	if (CashedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsFastSwimming() || CashedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsSwimming())
	{
		Oxygen -= SwimOxygenConsumptionVelocity * DeltaTime;
		Oxygen = FMath::Clamp(Oxygen, 0.0f, MaxOxygen);
		if (FMath::IsNearlyZero(Oxygen, 1e-6f))
		{
			if (OnOutOfOxygenEvent.IsBound())
			{
				OnOutOfOxygenEvent.Broadcast(true);
			}
		}
	}
	else
	{
		Oxygen += OxygenRestoreVelocity * DeltaTime;
		Oxygen = FMath::Clamp(Oxygen, 0.0f, MaxOxygen);
		if (Oxygen > 0)
		{
			if (OnOutOfOxygenEvent.IsBound())
			{
				OnOutOfOxygenEvent.Broadcast(false);
			}
		}
	}
}

void UCharacterAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateStaminaValue(DeltaTime);
	UpdateOxygenValue(DeltaTime);

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DebugDrawAttributes();
#endif

}

