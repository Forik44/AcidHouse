#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AHBasePlayerController.generated.h"

class UPlayerHUDWidget;
UCLASS()
class ACIDHOUSE_API AAHBasePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void SetPawn(APawn* InPawn) override;

	bool GetIgnoreCameraPitch() const;
	void SetIgnoreCameraPitch(bool bIgnoreCameraPitch_In);

protected:
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;

private:
	TSoftObjectPtr<class AAHBaseCharacter> CachedBaseCharacter;

	UPlayerHUDWidget* PlayerHUDWidget = nullptr;

	void CreateAndInitializeWidgets();

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void Jump();
	void ChangeCrouchState();
	void ChangeProneState();

	void StartSprint();
	void StopSprint();

	void SwimForward(float Value);
	void SwimRight(float Value);
	void SwimUp(float Value);

	void StartFastSwim();
	void StopFastSwim();

	void Mantle();

	void ClimbLadderUp(float Value);
	void InteractWithLadder();

	void InteractWithZipline();

	void PlayerStartFire();
	void PlayerStopFire();

	void StartAiming();
	void StopAiming();

	void Reload();

	bool bIgnoreCameraPitch = false;
};
