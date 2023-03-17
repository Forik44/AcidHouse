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

	float GetLadderHeight() const { return LadderHeight; }

protected:
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

	class UBoxComponent* GetLadderInteractionBox() const;

};
