// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/InteractiveActor.h"
#include "Zipline.generated.h"

class UStaticMeshComponent;
/**
 * 
 */
UCLASS(Blueprintable)
class ACIDHOUSE_API AZipline : public AInteractiveActor
{
	GENERATED_BODY()

public:
	AZipline();

	virtual void OnConstruction(const FTransform& Transform) override;

	FORCEINLINE FVector GetZiplineDirection() const { return ZiplineDirection; }
	FORCEINLINE float GetHeight() const { return ZiplineRailsHeight; }

	float GetCableLength() const;
protected:
	virtual void BeginPlay() override;

	virtual void OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline parametrs", meta = (ClampMin = 0, UIMin = 0))
	float ZiplineRailsHeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline parametrs", meta = (ClampMin = 0, UIMin = 0))
	float ZiplineCapsuleRadius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* FirstRailMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* SecondRailMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* CableMeshComponent;

	class UCapsuleComponent* GetZiplineInteractionCapsule() const;

private:
	float OldZiplineRailsHeight = ZiplineRailsHeight;

	FVector GetCableDifference(FVector FirstRailRelativeLocation, FVector SecondRailRelativeLocation) const;

	FVector ZiplineDirection = FVector::ZeroVector;
};
