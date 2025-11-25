// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "dazAda/Game/Characters/AcBaseCharacter.h"

#include "AcPlayerCharacter.generated.h"

struct FInputActionInstance;
class AAcPlayerController;
class AAcPlayerState;
class UInputMappingContext;
class UInputAction;


UCLASS()
class DAZADA_API AAcPlayerCharacter : public AAcBaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAcPlayerCharacter();

	UFUNCTION(BlueprintCallable, Category = "Arcane|Character")
	AAcPlayerController* GetAcPlayerController() const;
	
	UFUNCTION(BlueprintCallable, Category = "Arcane|Character")
	AAcPlayerState* GetAcPlayerState() const;
	USkeletalMeshComponent* GetWeaponComponent() const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void Turn(float Value);

	void BindASCInput();


	
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Arcane|Input")
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Arcane|Input")
	UInputAction* IA_Move;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Arcane|Input")
	UInputAction* IA_Jog;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Arcane|Input")
	UInputAction* IA_Sprint;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Arcane|Input")
	UInputAction* IA_LockCamera;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Arcane|Input")
	UInputAction* IA_Ability1;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Arcane|Input")
	UInputAction* IA_Ability2;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Arcane|Input")
	UInputAction* IA_Ability3;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Arcane|Input")
	UInputAction* IA_Ability4;

	void HandleMove(const FInputActionInstance& Instance);
	void HandleLockCamera(const FInputActionInstance& Instance);
	void HandleJogStart(const FInputActionInstance& Instance);
	void HandleJogEnd(const FInputActionInstance& Instance);
	void HandleSprintStart(const FInputActionInstance& Instance);
	void HandleSprintEnd(const FInputActionInstance& Instance);
	void HandleAbility1(const FInputActionInstance& Instance);
	void HandleAbility2(const FInputActionInstance& Instance);
	void HandleAbility3(const FInputActionInstance& Instance);
	void HandleAbility4(const FInputActionInstance& Instance);
	bool ASCInputBound = false;

	virtual void OnDeath() override;
public:
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arcane|Camera")
	bool lockCameraToHead = false;
	void LockCameraToHead(bool bLockCameraToHead);

	UFUNCTION(BlueprintCallable, Category =  "Arcane|Camera")
	void SetPlayerLockCamera(bool value);

	UFUNCTION(BlueprintCallable, Category="Arcane|Character")
	void DisablePlayerInput();

	UFUNCTION(BlueprintCallable, Category="Arcane|Character")
	void EnablePlayerInput();

	
private:
	void InitAbilitySystemComponent();
	void RotateToCamera();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arcane|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> ThirdPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arcane|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;


	// === Input Lock (Anti-Opposite Movement) ===
	float LastForwardKeyTime = -1.f;
	float LastRightKeyTime = -1.f;
	int32 LastForwardDir = 0; // +1 = W, -1 = S
	int32 LastRightDir = 0;   // +1 = D, -1 = A

	UPROPERTY(EditAnywhere, Category="Input|AntiReverse")
	float OppositeInputLockTime = 0.25f; // seconds
	bool bInputWasEnabled = true;
};


