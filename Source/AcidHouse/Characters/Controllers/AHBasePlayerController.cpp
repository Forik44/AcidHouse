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
	InputComponent->BindAxis("SwimForward", this, &AAHBasePlayerController::SwimForward);
	InputComponent->BindAxis("SwimRight", this, &AAHBasePlayerController::SwimRight);
	InputComponent->BindAxis("SwimUp", this, &AAHBasePlayerController::SwimUp);
	InputComponent->BindAxis("ClimbLadderUp", this, &AAHBasePlayerController::ClimbLadderUp);
	InputComponent->BindAction("InteractWithLadder", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::InteractWithLadder);
	InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::Mantle);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::Jump);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::StartSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &AAHBasePlayerController::StopSprint);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::ChangeCrouchState);
	InputComponent->BindAction("Prone", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::ChangeProneState);
	InputComponent->BindAction("FastSwim", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::StartFastSwim);
	InputComponent->BindAction("FastSwim", EInputEvent::IE_Released, this, &AAHBasePlayerController::StopFastSwim);
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
		CachedBaseCharacter->TryJump();
	}
}

void AAHBasePlayerController::ChangeCrouchState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeCrouchState();
	}
}

void AAHBasePlayerController::ChangeProneState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeProneState();
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

void AAHBasePlayerController::SwimForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimForward(Value);
	}
}

void AAHBasePlayerController::SwimRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimRight(Value);
	}
}

void AAHBasePlayerController::SwimUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimUp(Value);
	}
}

void AAHBasePlayerController::StartFastSwim()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartFastSwim();
	}
}

void AAHBasePlayerController::StopFastSwim()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopFastSwim();
	}
}

void AAHBasePlayerController::Mantle()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Mantle();
	}
}

void AAHBasePlayerController::ClimbLadderUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ClimbLadderUp(Value);
	}
}

void AAHBasePlayerController::InteractWithLadder()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithLadder();
	}
}
