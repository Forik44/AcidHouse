#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/LedgeDetectorComponent.h"
#include "AHBaceCharacerMovementComponent.generated.h"

struct FMantlingMovementParameters
{
	FVector InitialLocation = FVector::ZeroVector;
	FRotator InitialRotation = FRotator::ZeroRotator;

	FVector TargetLocation = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;

	FVector InitialAimationLocation = FVector::ZeroVector;

	float Duration = 1.0f;
	float StartTime = 0.0f;

	UCurveVector* MantlingCurve;

	TWeakObjectPtr<class AActor> LedgeActor;
	
	FVector TargetOffset = FVector::ZeroVector;
};

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	CMOVE_None = 0 UMETA(DisplayName = "None"),
	CMOVE_Mantling UMETA(DisplayName = "Mantling"),
	CMOVE_Ladder UMETA(DisplayName = "Ladder"),
	CMOVE_Zipline UMETA(DisplayName = "Zipline"),
	CMOVE_Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EDetachFromLadderMethod : uint8
{
	Fall = 0,
	ReachingTheTop,
	ReachingTheBottom,
	JumpOff
};
/**
 * 
 */
UCLASS()
class ACIDHOUSE_API UAHBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend class FSavedMove_AH;

public:
	bool bWantsToProne = false;

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual void PhysicsRotation(float DeltaTime) override;

	virtual float GetMaxSpeed() const override;

	void StartSprint();
	void StopSprint();

	bool CanEverSprint();

	void StartFastSwim();
	void StopFastSwim();

	virtual bool CanMeleeAttack();

	virtual bool CanFire();

	virtual bool CanCrouchInCurrentState() const override;

	virtual bool CanAttemptJump() const override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	void SetIsOutOfStamina(bool bIsOutOfStamina_In);

	virtual bool CanProneInCurrentState() const;
	virtual void Prone();
	virtual void UnProne();

	void StartMantle(const FMantlingMovementParameters& MantlingParameters);
	void EndMantle();
	bool IsMantling() const;
	virtual bool CanAttemptMantle() const;

	void AttachToLadder(const class ALadder* Ladder);
	void DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod = EDetachFromLadderMethod::Fall);

	void AttachToZipline(const class AZipline* Zipline);
	void DetachFromZipline();

	float GetActorToCurrentLadderProjection(const FVector& Location) const;
	float GetActorToCurrentZiplineProjection(const FVector& Location) const;

	bool IsOnLadder() const;
	float GetLadderSpeedRation() const;

	bool IsOnZipline() const;

	FORCEINLINE bool IsSprinting() const { return bIsSprinting; }
	bool IsProning() const;
	FORCEINLINE bool CanEverProne() const { return bCanEverProne; }
	FORCEINLINE bool IsFastSwimming() const { return bIsFastSwimming; }
	FORCEINLINE bool IsOutOfStamina() const { return bIsOutOfStamina; }

	FORCEINLINE float GetSwimmingCapsuleHalfHeight() const { return SwimmingCapsuleHalfHeight; }
	FORCEINLINE float GetSwimmingCapsuleRadius() const { return SwimmingCapsuleRadius; }

	FORCEINLINE const class ALadder* GetCurrentLadder() const { return CurrentLadder; }
	FORCEINLINE const class AZipline* GetCurrentZipline() const { return CurrentZipline; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SprintSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float OutOfStaminaSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: prone")
	bool bCanEverProne = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: prone", meta = (ClampMin = 0, UIMin = 0))
	float ProneCapsuleRadus = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: prone", meta = (ClampMin = 0, UIMin = 0))
	float ProneCapsuleHalfHeight = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: prone", meta = (ClampMin = 0, UIMin = 0))
	float MaxProneSpeed = 100.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, AdvancedDisplay, Category = "Character Movement (General Settings)")
	uint8 bProneMaintainsBaseLocation:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Swimming", meta = (ClampMin = "0", UIMin = "0"))
	float SwimmingCapsuleRadius = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Swimming", meta = (ClampMin = "0", UIMin = "0"))
	float SwimmingCapsuleHalfHeight = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Swimming", meta=(ClampMin="0", UIMin="0"))
	float FastSwimmingSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Swimming")
	bool bCanEverSwim = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Ladder", meta = (ClampMin = "0", UIMin = "0"))
	float ClimbingOnLadderMaxSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Ladder", meta = (ClampMin = "0", UIMin = "0"))
	float ClimbingOnLadderBreakingDecelaration = 2048.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Ladder", meta = (ClampMin = "0", UIMin = "0"))
	float LadderToCharacterOffset = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Ladder", meta = (ClampMin = "0", UIMin = "0"))
	float MaxLadderTopOffset = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Ladder", meta = (ClampMin = "0", UIMin = "0"))
	float MinLadderBottomOffset = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Ladder", meta = (ClampMin = "0", UIMin = "0"))
	float JumpOffFromLadderSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Zipline", meta = (ClampMin = "0", UIMin = "0"))
	float ZiplineMaxSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Zipline", meta = (ClampMin = "0", UIMin = "0"))
	float ZiplineBreakingDecelaration = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement: Zipline")
	float ZiplineToCharacterOffset = -50.0f;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviusCustomMode) override;

	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	void PhysMantling(float DeltaTime, int32 Iterations);
	void PhysLadder(float DeltaTime, int32 Iterations);
	void PhysZipline(float DeltaTime, int32 Iterations);

	FORCEINLINE class AAHBaseCharacter* GetBaseCharacterOwner() const { return CachedAHBaseCharacter; };

private:
	bool bIsSprinting = false;
	bool bIsOutOfStamina = false;
	bool bIsFastSwimming = false;

	class AAHBaseCharacter* CachedAHBaseCharacter;

	FMantlingMovementParameters CurrentMantlingParametrs;

	FTimerHandle MantlingTimer;

	const ALadder* CurrentLadder = nullptr;

	FRotator ForceTargetRotation = FRotator::ZeroRotator;
	bool bForceRotation = false;

	const AZipline* CurrentZipline = nullptr;
	FVector ZiplineDirection;
};

class FSavedMove_AH : public FSavedMove_Character
{
	typedef FSavedMove_Character Super;

public:
	virtual void Clear() override;

	virtual uint8 GetCompressedFlags() const override;

	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const override;

	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;

	virtual void PrepMoveFor(ACharacter* C) override;
private:
	uint8 bSavedIsSprinting : 1;
	uint8 bSavedIsMantling : 1;
};

class FNetworkPredictionData_Client_Character_AH : public FNetworkPredictionData_Client_Character
{
	typedef FNetworkPredictionData_Client_Character Super;

public:
	FNetworkPredictionData_Client_Character_AH(const UAHBaseCharacterMovementComponent& ClientMovement);

	virtual FSavedMovePtr AllocateNewMove() override;
};
