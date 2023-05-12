// Fill out your copyright notice in the Description page of Project Settings.


#include "Zipline.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "AcidHouseTypes.h"

AZipline::AZipline()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ZiplineRoot"));

	FirstRailMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FirstRail"));
	FirstRailMeshComponent->SetupAttachment(RootComponent);

	SecondRailMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondRail"));
	SecondRailMeshComponent->SetupAttachment(RootComponent);

	CableMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cable"));
	CableMeshComponent->SetupAttachment(RootComponent);
	CableMeshComponent->SetCollisionProfileName(FName("NoCollision"));

	InteractionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfilePawnInteractionVolume);
	InteractionVolume->SetGenerateOverlapEvents(true);

}

void AZipline::OnConstruction(const FTransform& Transform)
{
	FVector FirstRailRelativeLocation = FirstRailMeshComponent->GetRelativeLocation();
	FVector SecondRailRelativeLocation = SecondRailMeshComponent->GetRelativeLocation();
	if (OldZiplineRailsHeight != 1.0f)
	{
		FirstRailMeshComponent->SetRelativeLocation(FVector(FirstRailRelativeLocation.X, FirstRailRelativeLocation.Y, FirstRailRelativeLocation.Z - OldZiplineRailsHeight * 0.5f + ZiplineRailsHeight * 0.5f));
		SecondRailMeshComponent->SetRelativeLocation(FVector(SecondRailRelativeLocation.X, SecondRailRelativeLocation.Y, SecondRailRelativeLocation.Z - OldZiplineRailsHeight * 0.5f + ZiplineRailsHeight * 0.5f));
	}
	OldZiplineRailsHeight = ZiplineRailsHeight;


	UStaticMesh* FirstRailMesh = FirstRailMeshComponent->GetStaticMesh();
	if (IsValid(FirstRailMesh))
	{
		float MeshHeight = FirstRailMesh->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHeight))
		{
			FirstRailMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, ZiplineRailsHeight / MeshHeight));
		}
	}

	UStaticMesh* SecondRailMesh = SecondRailMeshComponent->GetStaticMesh();
	if (IsValid(SecondRailMesh))
	{
		float MeshHeight = SecondRailMesh->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHeight))
		{
			SecondRailMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, ZiplineRailsHeight / MeshHeight));
		}
	}

	FVector FirstRailTop = FVector(FirstRailRelativeLocation.X, FirstRailRelativeLocation.Y, FirstRailRelativeLocation.Z + ZiplineRailsHeight * 0.5f);
	FVector SecondRailTop = FVector(SecondRailRelativeLocation.X, SecondRailRelativeLocation.Y, SecondRailRelativeLocation.Z + ZiplineRailsHeight * 0.5f);

	FVector CableDifference = GetCableDifference(FirstRailTop, SecondRailTop);
	if (FirstRailTop.Z > SecondRailTop.Z)
	{
		CableMeshComponent->SetRelativeLocation(SecondRailTop + CableDifference * 0.5f);
		InteractionVolume->SetRelativeLocation(SecondRailTop + CableDifference * 0.5f);
	}
	else 
	{
		CableMeshComponent->SetRelativeLocation(FirstRailTop + CableDifference * 0.5f);
		InteractionVolume->SetRelativeLocation(FirstRailTop + CableDifference * 0.5f);
	}

	FRotator CableOrientation = CableDifference.ToOrientationRotator();
	CableOrientation.Pitch += 90.0f;

	CableMeshComponent->SetRelativeRotation(CableOrientation);
	InteractionVolume->SetRelativeRotation(CableOrientation);

	UStaticMesh* CableMesh = CableMeshComponent->GetStaticMesh();
	if (IsValid(CableMesh))
	{
		float MeshHeight = CableMesh->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(CableDifference.Size()))
		{
			CableMeshComponent->SetRelativeScale3D(FVector(1.0f, 1.0f, CableDifference.Size() / MeshHeight));
		}
	}

	GetZiplineInteractionCapsule()->SetCapsuleHalfHeight(CableDifference.Size() * 0.5f);
	GetZiplineInteractionCapsule()->SetCapsuleRadius(ZiplineCapsuleRadius);
}

float AZipline::GetCableLength() const
{
	return ZiplineDirection.Size();
}

void AZipline::BeginPlay()
{
	Super::BeginPlay();

	FVector FirstRailRelativeLocation = FirstRailMeshComponent->GetRelativeLocation();
	FVector SecondRailRelativeLocation = SecondRailMeshComponent->GetRelativeLocation();

	FVector FirstRailTop = FVector(FirstRailRelativeLocation.X, FirstRailRelativeLocation.Y, FirstRailRelativeLocation.Z + ZiplineRailsHeight * 0.5f);
	FVector SecondRailTop = FVector(SecondRailRelativeLocation.X, SecondRailRelativeLocation.Y, SecondRailRelativeLocation.Z + ZiplineRailsHeight * 0.5f);

	ZiplineDirection = GetCableDifference(FirstRailTop, SecondRailTop);
}

FVector AZipline::GetCableDifference(FVector FirstRailRelativeLocation, FVector SecondRailRelativeLocation) const
{

	FVector CableDifference = FVector::ZeroVector;
	if (FirstRailRelativeLocation.Z > SecondRailRelativeLocation.Z)
	{
		CableDifference = FirstRailRelativeLocation - SecondRailRelativeLocation;
	}
	else
	{
		CableDifference = SecondRailRelativeLocation - FirstRailRelativeLocation;
	}

	return CableDifference;
}

void AZipline::OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnInteractionVolumeOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}
}

void AZipline::OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnInteractionVolumeOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}


}

UCapsuleComponent* AZipline::GetZiplineInteractionCapsule() const
{
	return StaticCast<UCapsuleComponent*>(InteractionVolume);
}
