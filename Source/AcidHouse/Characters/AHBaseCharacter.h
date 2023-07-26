// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AcidHouseTypes.h"
#include "GenericTeamAgentInterface.h"
#include "AHBaseCharacter.generated.h"

USTRUCT(BlueprintType)
struct FMantlingSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* MantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* FPMantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UCurveVector* MantlingCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0, UIMin = 0))
	float AnimationCorrectionXY = 65.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0, UIMin = 0))
	float AnimationCorrectionZ = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0, UIMin = 0))
	float MaxHeight = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0, UIMin = 0))
	float MinHeight = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0, UIMin = 0))
	float MaxHeightStartTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0, UIMin = 0))
	float MinHeightStartTime = 0.5f;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAimingStateChanged, bool)

class AInteractiveActor;
class UAHBaseCharacterMovementComponent;
class UCharacterEquipmentComponent;
class UCharacterAttributeComponent;

typedef TArray<AInteractiveActor*, TInlineAllocator<10>> TInteractiveActorsArray;

UCLASS(Abstract, NotBlueprintable)
class ACIDHOUSE_API AAHBaseCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AAHBaseCharacter(const FObjectInitializer& ObjectInitializer);

	bool bIsProning = false;

	FRotator GetAimOffset();

	FOnAimingStateChanged OnAimingStateChanged;

	virtual void StopJumping() override;

	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void MoveForward(float Value) {};
	virtual void MoveRight(float Value) {};
	virtual void Turn(float Value) {};
	virtual void LookUp(float Value) {};

	virtual void SwimForward(float Value) {};
	virtual void SwimRight(float Value) {};
	virtual void SwimUp(float Value) {};

	virtual void TryJump();

	virtual void ChangeCrouchState();
	virtual void ChangeProneState();

	virtual void Prone();
	virtual void UnProne();

	virtual void OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartProne", ScriptName="OnStartProne"))
	void K2_OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	virtual void OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartProne", ScriptName="OnStartProne"))
	void K2_OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	virtual void StartSprint();
	virtual void StopSprint();

	virtual void StartFastSwim();
	virtual void StopFastSwim();

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSwimStart();
	virtual void OnSwimStart_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSwimEnd();
	virtual void OnSwimEnd_Implementation();

	UFUNCTION(BlueprintCallable)
	void Mantle(bool bForce = false);

	UPROPERTY(ReplicatedUsing = OnRep_IsMantling)
	bool bIsMantling;

	UPROPERTY(ReplicatedUsing = OnRep_IsOnLadder)
	bool bIsOnLadder;

	UPROPERTY(ReplicatedUsing = OnRep_IsOnZipline)
	bool bIsOnZipline;

	UPROPERTY(Replicated)
	bool bIsOutOfStamina = false;

	UFUNCTION()
	void OnRep_IsMantling(bool bWasMantling);

	UFUNCTION()
	void OnRep_IsOnLadder(bool bWasOnLadder);

	UFUNCTION()
	void OnRep_IsOnZipline(bool bWasOnZipline);

	void ClimbLadderUp(float Value);
	void InteractWithLadder();
	const class ALadder* GetAvailableLadder() const;

	void InteractWithZipline();
	const class AZipline* GetAvailableZipline() const;

	void StartFire();

	bool CanFire();

	void StopFire();

	void StartAiming();
	void StopAiming();
	bool IsAiming() const { return bIsAiming; }
	float GetAimingMovementSpeed() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStartAiming();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStopAiming();

	void Reload();

	void NextItem();
	void PreviousItem();

	void EquipPrimaryItem();

	void PrimaryMeleeAttack();
	void SecondaryMeleeAttack();
	bool IsMeleeAttacking();

	void RegisterInteractiveActor(AInteractiveActor* IntaractiveActor);
	void UnregisterInteractiveActor(AInteractiveActor* IntaractiveActor);

	virtual void Falling() override;
	virtual void NotifyJumpApex() override;
	virtual void Landed(const FHitResult& Hit) override;

	/** IGenericTeamAgentInterface */
	virtual FGenericTeamId GetGenericTeamId() const override;
	/** ~IGenericTeamAgentInterface */

	FORCEINLINE UAHBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() const { return AHBaseCharacterMovementComponent; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKRightFootOffset() const { return IKRightFootOffset; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE float GetIKLeftFootOffset() const { return IKLeftFootOffset; }

	FORCEINLINE  UCharacterAttributeComponent* GetCharacterAttributeComponent_Mutable() const { return CharacterAttributeComponent; }

	FORCEINLINE const UCharacterEquipmentComponent* GetCharacterEquipmentComponent() const { return CharacterEquipmentComponent; }
	FORCEINLINE  UCharacterEquipmentComponent* GetCharacterEquipmentComponent_Mutable() const { return CharacterEquipmentComponent; }

	FORCEINLINE const UCharacterAttributeComponent* GetCharacterAttributeComponent() const { return CharacterAttributeComponent; }


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement")
	float SprintSpeed = 800.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK settings")
	FName RightFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK settings")
	FName LeftFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | IK settings", meta = (ClampMin = 0, UIMin = 0))
	float IKTraceExtedDistance = 30.0f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Character | IK settings", meta = (ClampMin = 0, UIMin = 0))
	float IKInterpSpeed = 20.0f; 

	UAHBaseCharacterMovementComponent* AHBaseCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	class ULedgeDetectorComponent* LedgeDetectorComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling")
	FMantlingSettings HighMantleSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling")
	FMantlingSettings LowMantleSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float LowMantleMaxHeight = 125.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	UCharacterAttributeComponent* CharacterAttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	UCharacterEquipmentComponent* CharacterEquipmentComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Animations")
	class UAnimMontage* OnDeathAnimMontage;

	//Damage depending from fall height (in meters)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Attributes")
	class UCurveFloat* FallDamageCurve; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Team")
	ETeams Team = ETeams::Enemy;

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSprintStart();
	virtual void OnSprintStart_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSprintEnd();
	virtual void OnSprintEnd_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnFastSwimStart();
	virtual void OnFastSwimStart_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnFastSwimEnd();
	virtual void OnFastSwimEnd_Implementation();

	virtual bool CanSprint();
	virtual bool CanFastSwim();
	virtual bool CanMantle();
	virtual bool CanProne();

	virtual bool CanJumpInternal_Implementation() const override;

	virtual void OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime);

	virtual void OnDeath();

	UFUNCTION()
	virtual void OnOutOfStamina(bool IsOutOfStamina);

	UFUNCTION()
	virtual void OnOutOfOxygen(bool IsOutOfOxygen);

	virtual void OnStartAimingInternal();
	virtual void OnStopAimingInternal();
private:
	bool bIsSprintRequested = false;
	bool bIsFastSwimRequested = false;

	UPROPERTY(ReplicatedUsing=OnRep_bIsAiming)
	bool bIsAiming = false;

	UPROPERTY(ReplicatedUsing=OnRep_bIsMeleeAttacking)
	bool bIsMeleeAttacking = false;

	UPROPERTY(Replicated)
	EMeleeAttackTypes CurrentMeleeAttackType = EMeleeAttackTypes::None;

	float CurrentAimingMovementSpeed = 0.0f;

	float IKRightFootOffset = 0.0f;
	float IKLeftFootOffset = 0.0f;
	
	float IKTraceDistance = 0.0f;

	FVector CurrentFallApex;

	FTimerHandle OutOfOxygenTimer;

	TInteractiveActorsArray AvailableInteractiveActors;

	UFUNCTION()
	void OnMeleeAttackEnded();

	UFUNCTION()
	void OnRep_bIsMeleeAttacking(bool bWasMeleeAttacking);

	UFUNCTION()
	void OnRep_bIsAiming(bool bWasAiming);

	UFUNCTION(Server, Reliable)
	void Server_StartAiming();

	UFUNCTION(Server, Reliable)
	void Server_StopAiming();

	UFUNCTION(Server, Reliable)
	void Server_StartMeleeAttack(EMeleeAttackTypes MeleeAttackType);

	UFUNCTION(Server, Reliable)
	void Server_EndMeleeAttack();

	UFUNCTION(Server, Reliable)
	void Server_SetCurrentMeleeAttackType(EMeleeAttackTypes MeleeAttackType);

	void TryChangeSprintState(float DeltaTime);
	void TryChangeFastSwimState(float DeltaTime);

	void OutOfOxygenDamage();

	float GetIKOffsetForASocket(const FName& SocketName);

	const FMantlingSettings& GetMantlingSettings(float LedgeHeight) const;

	void EnableRagdoll();

	void StartMeleeAttackByType(EMeleeAttackTypes MeleeAttackType);
	

};
