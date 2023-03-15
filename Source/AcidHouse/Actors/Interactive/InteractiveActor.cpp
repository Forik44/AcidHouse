#include "InteractiveActor.h"
#include "../../Characters/AHBaseCharacter.h"
#include "Components/CapsuleComponent.h"

void AInteractiveActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(InteractionVolume))
	{
		InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &AInteractiveActor::OnInteractionVolumeOverlapBegin);
		InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &AInteractiveActor::OnInteractionVolumeOverlapEnd);
	}
}

void AInteractiveActor::OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAHBaseCharacter* BaseCharacter = Cast<AAHBaseCharacter>(OtherActor);
	if (!IsValid(BaseCharacter))
	{
		return;
	}

	if (Cast<UCapsuleComponent>(OtherComp) != BaseCharacter->GetCapsuleComponent())
	{
		return;
	}

	BaseCharacter->RegisterInteractiveActor(this);
}

void AInteractiveActor::OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AAHBaseCharacter* BaseCharacter = Cast<AAHBaseCharacter>(OtherActor);
	if (!IsValid(BaseCharacter))
	{
		return;
	}

	if (Cast<UCapsuleComponent>(OtherComp) != BaseCharacter->GetCapsuleComponent())
	{
		return;
	}

	BaseCharacter->UnregisterInteractiveActor(this); 
}

