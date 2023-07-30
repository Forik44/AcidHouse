#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AICharacterSpawner.generated.h"

class AAHAICharacter;
UCLASS()
class ACIDHOUSE_API AAICharacterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AAICharacterSpawner();

	UFUNCTION()
	void SpawnAI();

protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Spawner")
	TSubclassOf<AAHAICharacter> CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Spawner")
	bool bIsSpawnOnStart = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Spawner")
	bool bDoOnce = false;

	// An actor implementing IInteractible interface
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Spawner")
	AActor* SpawnTriggerActor;

private:
	bool bCanSpawn = true;

	void UnSubscribeFromTrigger();

	UPROPERTY()
	TScriptInterface<IInteractable> SpawnTrigger;

	FDelegateHandle TriggetHandle;
};
