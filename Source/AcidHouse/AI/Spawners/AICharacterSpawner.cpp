#include "AI/Spawners/AICharacterSpawner.h"
#include "AI/Characters/AHAICharacter.h"
#include "Actors/Interactive/Interface/IInteractable.h"


AAICharacterSpawner::AAICharacterSpawner()
{
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnerRoot"));
	SetRootComponent(SceneRoot);
}

void AAICharacterSpawner::SpawnAI()
{
	if (!bCanSpawn || !IsValid(CharacterClass))
	{
		return;
	}

	AAHAICharacter* AICharacter = GetWorld()->SpawnActor<AAHAICharacter>(CharacterClass, GetTransform());\
	
	if (!IsValid(AICharacter))
	{
		return;
	}

	if (!IsValid(AICharacter->Controller))
	{
		AICharacter->SpawnDefaultController();
	}

	if (bDoOnce)
	{
		UnSubscribeFromTrigger();
		bCanSpawn = false;
	}
}

void AAICharacterSpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property->GetName() == GET_MEMBER_NAME_STRING_CHECKED(AAICharacterSpawner, SpawnTriggerActor))
	{
		SpawnTrigger = SpawnTriggerActor;
		if (SpawnTrigger.GetInterface())
		{
			if (!SpawnTrigger->HasOnInteractionCallback())
			{
				SpawnTriggerActor = nullptr;
				SpawnTrigger = nullptr;
			}
		}
		else
		{
			SpawnTriggerActor = nullptr;
			SpawnTrigger = nullptr;
		}
	}
}

void AAICharacterSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnTrigger.GetInterface())
	{
		TriggetHandle = SpawnTrigger->AddOnInteractionUFunction(this, FName("SpawnAI"));
	}

	if (bIsSpawnOnStart)
	{
		SpawnAI();
	}
}

void AAICharacterSpawner::EndPlay(const EEndPlayReason::Type Reason)
{
	UnSubscribeFromTrigger();

	Super::EndPlay(Reason);
}

void AAICharacterSpawner::UnSubscribeFromTrigger()
{
	if (TriggetHandle.IsValid() && SpawnTrigger.GetInterface())
	{
		SpawnTrigger->RemoveOnInteractionDelegate(TriggetHandle);
	}
}

