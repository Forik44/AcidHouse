#pragma once

#include "CoreMinimal.h"
#include "../InteractiveActor.h"
#include "Ladder.generated.h"

class UStaticMeshComponent;
/**
 * 
 */
UCLASS(Blueprintable)
class ACIDHOUSE_API ALadder : public AInteractiveActor
{
	GENERATED_BODY()

public:
	ALadder();

	virtual void OnConstruction(const FTransform& Transform) override;

	FORCEINLINE float GetLadderHeight() const { return LadderHeight; }
	FORCEINLINE bool GetIsOnTop() const { return bIsOnTop; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parametrs", meta = (ClampMin = 0, UIMin = 0))
	float LadderHeight = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parametrs", meta = (ClampMin = 0, UIMin = 0))
	float LadderWidth = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parametrs", meta = (ClampMin = 0, UIMin = 0))
	float StepsInterval = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parametrs", meta = (ClampMin = 0, UIMin = 0))
	float BottomStepOffset = 25.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* RightRailMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LeftRailMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInstancedStaticMeshComponent* StepsMeshComponent; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* TopInteractionVolume;

	class UBoxComponent* GetLadderInteractionBox() const;

	virtual void OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:
	bool bIsOnTop = false;
};
