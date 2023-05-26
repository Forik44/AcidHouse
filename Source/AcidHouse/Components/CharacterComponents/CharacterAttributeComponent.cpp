#include "CharacterAttributeComponent.h"
#include "Characters/AHBaseCharacter.h"
#include "Subsystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "AcidHouseTypes.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/MovementComponents/AHBaceCharacerMovementComponent.h"

UCharacterAttributeComponent::UCharacterAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCharacterAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(MaxHealth > 0.0f, TEXT("UCharacterAttributeComponent::BeginPlay() max health can't be equal to 0"));
	checkf(GetOwner()->IsA<AAHBaseCharacter>(), TEXT("UCharacterAttributeComponent::BeginPlay() Can be used only with AAHBaseCharacter"));
	CashedBaseCharacterOwner = StaticCast<AAHBaseCharacter*>(GetOwner());

	SetHealth(MaxHealth);
	SetStamina(MaxStamina);
	SetOxygen(MaxOxygen);

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
	float NewHealth = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	SetHealth(NewHealth);
}

void UCharacterAttributeComponent::UpdateStaminaValue(float DeltaTime)
{
	if (!CashedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsSprinting() && !CashedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsFastSwimming())
	{
		float NewStamina = Stamina + StaminaRestoreVelocity * DeltaTime;
		NewStamina = FMath::Clamp(NewStamina, 0.0f, MaxStamina);
		SetStamina(NewStamina);
	}

	if (CashedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsSprinting())
	{
		float NewStamina = Stamina - StaminaRestoreVelocity * DeltaTime;
		NewStamina = FMath::Clamp(NewStamina, 0.0f, MaxStamina);
		SetStamina(NewStamina);
	}

	if (CashedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsFastSwimming())
	{
		float NewStamina = Stamina - SprintStaminaConsumptionVelocity * DeltaTime;
		NewStamina = FMath::Clamp(NewStamina, 0.0f, MaxStamina);
		SetStamina(NewStamina);
		if (FMath::IsNearlyZero(NewStamina, 1e-6f))
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
	float NewOxygen = 0.0f;
	if (CashedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsFastSwimming() || CashedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsSwimming())
	{
		NewOxygen = Oxygen - SwimOxygenConsumptionVelocity * DeltaTime;

	}
	else
	{
		NewOxygen = Oxygen + SwimOxygenConsumptionVelocity * DeltaTime;
	}
	NewOxygen = FMath::Clamp(NewOxygen, 0.0f, MaxOxygen);
	SetOxygen(NewOxygen);
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

void UCharacterAttributeComponent::SetHealth(float NewHealth)
{
	if (Health != NewHealth)
	{
		Health = NewHealth;
		OnHealthPersentChanged.Broadcast(Health / MaxHealth);
		if (Health <= 0.0f)
		{
			if (OnDeathEvent.IsBound())
			{
				OnDeathEvent.Broadcast();
			}
		}
	}
}

void UCharacterAttributeComponent::SetStamina(float NewStamina)
{
	if (Stamina != NewStamina)
	{
		Stamina = NewStamina;
		OnStaminaPersentChanged.Broadcast(Stamina / MaxStamina);

		if (Stamina == 0)
		{
			if (OnOutOfStaminaEvent.IsBound())
			{
				OnOutOfStaminaEvent.Broadcast(true);
			}
		}

		if (Stamina == MaxStamina)
		{
			if (OnOutOfStaminaEvent.IsBound())
			{
				OnOutOfStaminaEvent.Broadcast(false);
			}
		}
	}
}

void UCharacterAttributeComponent::SetOxygen(float NewOxygen)
{
	if (Oxygen != NewOxygen)
	{
		Oxygen = NewOxygen;
		OnOxygenPersentChanged.Broadcast(Oxygen / MaxOxygen);
	
		if (FMath::IsNearlyZero(Oxygen, 1e-6f))
		{
			if (OnOutOfOxygenEvent.IsBound())
			{
				OnOutOfOxygenEvent.Broadcast(true);
			}
		}
		else
		{
			if (OnOutOfOxygenEvent.IsBound())
			{
				OnOutOfOxygenEvent.Broadcast(false);
			}
		}

	}
}


