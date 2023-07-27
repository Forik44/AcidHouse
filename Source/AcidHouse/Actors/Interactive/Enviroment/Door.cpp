#include "Actors/Interactive/Enviroment/Door.h"
#include "Components/TimelineComponent.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	USceneComponent* DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorRoot"));
	SetRootComponent(DefaultSceneRoot);

	DoorPivot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorPivot"));
	DoorPivot->SetupAttachment(GetRootComponent());

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(DoorPivot);
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();
	

	if (IsValid(DoorAnimationCurve))
	{
		FOnTimelineFloatStatic DoorAnimationDelegate;
		DoorAnimationDelegate.BindUObject(this, &ADoor::UpdateDoorAnimation);
		DoorOpenAnimTimeline.AddInterpFloat(DoorAnimationCurve, DoorAnimationDelegate);

		FOnTimelineEventStatic DoorOpenedDelegate;
		DoorOpenedDelegate.BindUObject(this, &ADoor::OnDoorAnimationFinished);
		DoorOpenAnimTimeline.SetTimelineFinishedFunc(DoorOpenedDelegate);
	}
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DoorOpenAnimTimeline.TickTimeline(DeltaTime);
}

void ADoor::Interact(AAHBaseCharacter* Character)
{

	ensure(IsValid(DoorAnimationCurve), TEXT("DoorAnimationCurve is not set"));
	InteractWithDoor();
}

FName ADoor::GetActionEventName() const
{
	return ActionInteract;
}

void ADoor::UpdateDoorAnimation(float Alpha)
{
	float YawAngle = FMath::(AngleClosed, AngleOpened, FMath::Clamp(Alpha, 0.0f, 1.0f));
	DoorPivot->SetRelativeRotation(FRotator(0.0f, YawAngle, 0.0f));
}

void ADoor::OnDoorAnimationFinished()
{
	SetActorTickEnabled(false);
}

void ADoor::InteractWithDoor()
{
	SetActorTickEnabled(true);

	if (bIsOpened)
	{
		DoorOpenAnimTimeline.Reverse();
	}
	else
	{
		DoorOpenAnimTimeline.Play();
	}
	bIsOpened = !bIsOpened;
}

