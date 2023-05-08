#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "FPPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ACIDHOUSE_API AFPPlayerCharacter : public APlayerCharacter
{
	GENERATED_BODY()
public:
	AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PossessedBy(AController* NewController) override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual FRotator GetViewRotation() const override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First person")
	class USkeletalMeshComponent* FirstPersonMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | First person")
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Pitch", meta = (UIMin = -89.0f, UIMax = 89))
	float LadderCameraMinPitch = -60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Pitch", meta = (UIMin = -89.0f, UIMax = 89))
	float LadderCameraMaxPitch = 80.0f;

	UPROPERTY(EditDefaultsOnly,  BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Yaw", meta = (UIMin = -89.0f, UIMax = 89))
	float LadderCameraMinYaw = -60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | First person | Camera | Ladder | Yaw", meta = (UIMin = -89.0f, UIMax = 89))
	float LadderCameraMaxYaw = 60.0f;

	virtual void OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime) override;
	
private:
	FTimerHandle FPMontageTimer;

	TWeakObjectPtr<class AAHBasePlayerController> AHBasePlayerController;

	void OnFPMontageTimerElapsed();

	bool IsFPMontagePlaying() const;

	void OnLadderStoped();

	void OnLadderStarted();

};
