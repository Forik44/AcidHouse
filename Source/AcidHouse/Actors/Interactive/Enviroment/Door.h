#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/Interactive/Interface/IInteractable.h"
#include "Components/TimelineComponent.h"
#include "Door.generated.h"

UCLASS()
class ACIDHOUSE_API ADoor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	ADoor();

	virtual void Tick(float DeltaTime) override;

	virtual void Interact(AAHBaseCharacter* Character) override;
	virtual FName GetActionEventName() const override;

	virtual bool HasOnInteractionCallback() const override;
	virtual FDelegateHandle AddOnInteractionUFunction(UObject* Object, const FName& FunctionName) override;
	virtual void RemoveOnInteractionDelegate(FDelegateHandle DelegationHandle) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	USceneComponent* DoorPivot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	float AngleClosed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	float AngleOpened = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	UCurveFloat* DoorAnimationCurve;

	virtual void BeginPlay() override;

	IInteractable::FOnInteraction OnInteractionEvent;

private:
	FTimeline DoorOpenAnimTimeline;

	bool bIsOpened = false;

	UFUNCTION()
	void UpdateDoorAnimation(float Alpha);

	UFUNCTION()
	void OnDoorAnimationFinished();

	void InteractWithDoor();
};
