#include "CharacterAttributeComponent.h"
#include "../../Characters/AHBaseCharacter.h"
#include "../../Subsystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "../../AcidHouseTypes.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

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
	DrawDebugString(GetWorld(), TextLocation, FString::Printf(TEXT("Health: %.2f"), Health), nullptr, FColor::Green, 0.0f, true);

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

void UCharacterAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DebugDrawAttributes();
#endif

}

