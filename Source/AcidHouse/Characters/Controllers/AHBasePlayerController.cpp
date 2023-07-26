#include "AHBasePlayerController.h"
#include "Characters/AHBaseCharacter.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widgets/ReticleWidget.h"
#include "UI/Widgets/AmmoWidget.h"
#include "UI/Widgets/PlayerHUDWidget.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"
#include "UI/Widgets/CharacterAttributesWidget.h"

void AAHBasePlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	CachedBaseCharacter = Cast<AAHBaseCharacter>(InPawn); 

	if (IsLocalController())
	{
		PlayerCameraManager->DefaultFOV = DefaultPlayerFOV;
		CreateAndInitializeWidgets();
	}
}

bool AAHBasePlayerController::GetIgnoreCameraPitch() const
{
	return bIgnoreCameraPitch;
}

void AAHBasePlayerController::SetIgnoreCameraPitch(bool bIgnoreCameraPitch_In)
{
	bIgnoreCameraPitch = bIgnoreCameraPitch_In;
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
	InputComponent->BindAction("InteractWithZipline", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::InteractWithZipline);
	InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::Mantle);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::Jump);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::StartSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &AAHBasePlayerController::StopSprint);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::ChangeCrouchState);
	InputComponent->BindAction("Prone", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::ChangeProneState);
	InputComponent->BindAction("FastSwim", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::StartFastSwim);
	InputComponent->BindAction("FastSwim", EInputEvent::IE_Released, this, &AAHBasePlayerController::StopFastSwim);
	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::PlayerStartFire);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &AAHBasePlayerController::PlayerStopFire);
	InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::StartAiming);
	InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &AAHBasePlayerController::StopAiming);
	InputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::Reload);
	InputComponent->BindAction("NextItem", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::NextItem);
	InputComponent->BindAction("PreviousItem", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::PreviousItem);
	InputComponent->BindAction("EquipPrimaryItem", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::EquipPrimaryItem);
	InputComponent->BindAction("PrimaryMeleeAttack", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::PrimaryMeleeAttack);
	InputComponent->BindAction("SecondaryMeleeAttack", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::SecondaryMeleeAttack);
	FInputActionBinding& ToggleMenuBinding = InputComponent->BindAction("ToggleMainMenu", EInputEvent::IE_Pressed, this, &AAHBasePlayerController::ToggleMainMenu);
	ToggleMenuBinding.bExecuteWhenPaused = true;
}

void AAHBasePlayerController::CreateAndInitializeWidgets()
{
	if (!IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
		if (IsValid(PlayerHUDWidget))
		{
			PlayerHUDWidget->AddToViewport();
		}
	}

	if (!IsValid(MainMenuWidget))
	{
		MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
		if (IsValid(PlayerHUDWidget))
		{
			MainMenuWidget->AddToViewport();
		}
	}

	if (CachedBaseCharacter.IsValid() && IsValid(PlayerHUDWidget))
	{
		UReticleWidget* ReticleWidget = PlayerHUDWidget->GetReticleWidget();
		if (IsValid(ReticleWidget))
		{
			CachedBaseCharacter->OnAimingStateChanged.AddUFunction(ReticleWidget, FName("OnAimingStateChanged"));
			UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
			CharacterEquipment->OnEquippedItemChanged.AddUFunction(ReticleWidget, FName("OnEquippedItemChanged"));
		}

		UAmmoWidget* AmmoWidget = PlayerHUDWidget->GetAmmoWidget();
		if (IsValid(AmmoWidget))
		{
			UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
			CharacterEquipment->OnCurrentWeaponAmmoChangedEvent.AddUFunction(AmmoWidget, FName("UpdateAmmoCount"));
			CharacterEquipment->OnCurrentThrowableItemAmmoChanged.AddUFunction(AmmoWidget, FName("UpdateThrowableItemAmmoCount"));
		}

		UCharacterAttributesWidget* CharacterAttributesWidget = PlayerHUDWidget->GetCharacterAttributesWidget();
		if (IsValid(CharacterAttributesWidget))
		{
			UCharacterAttributeComponent* CharacterAttribute = CachedBaseCharacter->GetCharacterAttributeComponent_Mutable();
			CharacterAttribute->OnHealthPersentChanged.AddUFunction(CharacterAttributesWidget, FName("OnHealthPersentChanged"));
			CharacterAttribute->OnStaminaPersentChanged.AddUFunction(CharacterAttributesWidget, FName("OnStaminaPersentChanged"));
			CharacterAttribute->OnOxygenPersentChanged.AddUFunction(CharacterAttributesWidget, FName("OnOxygenPersentChanged"));
		}
	}

	MainMenuWidget->SetVisibility(ESlateVisibility::Hidden);
	PlayerHUDWidget->SetVisibility(ESlateVisibility::Visible);
	SetInputMode(FInputModeGameOnly{});
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

void AAHBasePlayerController::InteractWithZipline()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithZipline();
	}
}

void AAHBasePlayerController::PlayerStartFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartFire();
	}
}

void AAHBasePlayerController::PlayerStopFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopFire();
	}
}

void AAHBasePlayerController::StartAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartAiming();
	}
}

void AAHBasePlayerController::StopAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopAiming();
	}
}

void AAHBasePlayerController::NextItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->NextItem();
	}
}

void AAHBasePlayerController::PreviousItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PreviousItem();
	}
}

void AAHBasePlayerController::Reload()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Reload();
	}
}

void AAHBasePlayerController::EquipPrimaryItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->EquipPrimaryItem();
	}
}

void AAHBasePlayerController::PrimaryMeleeAttack()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PrimaryMeleeAttack();
	}
}

void AAHBasePlayerController::SecondaryMeleeAttack()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SecondaryMeleeAttack();
	}
}

void AAHBasePlayerController::ToggleMainMenu()
{
	if (!IsValid(MainMenuWidget) || !IsValid(PlayerHUDWidget))
	{
		return;
	}

	if (MainMenuWidget->IsVisible())
	{
		MainMenuWidget->SetVisibility(ESlateVisibility::Hidden);
		PlayerHUDWidget->SetVisibility(ESlateVisibility::Visible);
		SetInputMode(FInputModeGameOnly{});
		bShowMouseCursor = false;
		SetPause(false);
	}
	else
	{
		PlayerHUDWidget->SetVisibility(ESlateVisibility::Hidden);
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
		SetInputMode(FInputModeGameAndUI{});
		SetPause(true);
		bShowMouseCursor = true;
	}
}
