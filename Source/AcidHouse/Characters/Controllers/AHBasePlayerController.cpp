#include "AHBasePlayerController.h"
#include "../AHBaseCharacter.h"

void AAHBasePlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	CachedBaseCharacter = Cast<AAHBaseCharacter>(InPawn);
}

void AAHBasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &AAHBasePlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AAHBasePlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &AAHBasePlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &AAHBasePlayerController::LookUp);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::Jump);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::StartSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &AAHBasePlayerController::StopSprint);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::ChangeCrouchState);
}

void AAHBasePlayerController::MoveForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveForward(Value);
	}
}

void AAHBasePlayerController::MoveRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveRight(Value);
	}
}

void AAHBasePlayerController::Turn(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Turn(Value);
	}
}

void AAHBasePlayerController::LookUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUp(Value);
	}
}

void AAHBasePlayerController::Jump()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Jump();
	}
}

void AAHBasePlayerController::ChangeCrouchState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeCrouchState();
	}
}

void AAHBasePlayerController::StartSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartSprint();
	}
}

void AAHBasePlayerController::StopSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopSprint();
	}
}
