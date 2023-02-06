#include "AcidHouseBasePawn.h"
#include "Components/SphereComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/PawnMovementComponent.h"
#include "../Components/MovementComponents/AHBasePawnMovementComponent.h"
// Sets default values
AAcidHouseBasePawn::AAcidHouseBasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->SetSphereRadius(50.0f);
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	RootComponent = CollisionComponent;

	PawnMovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UAHBasePawnMovementComponent>(TEXT("MovementComponent"));
	PawnMovementComponent->SetUpdatedComponent(CollisionComponent);
}


void AAcidHouseBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("MoveForward", this, &AAcidHouseBasePawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAcidHouseBasePawn::MoveRight);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AAcidHouseBasePawn::Jump);
}

void AAcidHouseBasePawn::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AAcidHouseBasePawn::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AAcidHouseBasePawn::Jump()
{
	checkf(PawnMovementComponent->IsA<UAHBasePawnMovementComponent>(), TEXT("AAcidHouseBasePawn::Jump() can work only with UAHBasePawnMovementComponent"));
	UAHBasePawnMovementComponent* BaseMovement = StaticCast<UAHBasePawnMovementComponent*>(PawnMovementComponent);
	BaseMovement->JumpStart(); 
}

