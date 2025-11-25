// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "AcPlayerController.generated.h"
class AAcPlayerState;
class APawn;
class APlayerState;
class UAcAbilitySystemComponent;
class UObject;
class UPlayer;
/**
 * 
 */
UCLASS()
class DAZADA_API AAcPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AAcPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	UFUNCTION(BlueprintCallable, Category = "Arcane|PlayerController")
	AAcPlayerState* GetAcPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "Arcane|PlayerController")
	UAbilitySystemComponent* GetAcAbilitySystemComponent() const;


	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~End of AActor interface

	//~AController interface
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void InitPlayerState() override;
	virtual void CleanupPlayerState() override;
	virtual void OnRep_PlayerState() override;
	//~End of AController interface

	//~APlayerController interface
	virtual void ReceivedPlayer() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetPlayer(UPlayer* InPlayer) override;
	virtual void AddCheats(bool bForce) override;
	virtual void UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId) override;
	virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	/*virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;*/
	//~End of APlayerController interface
};
