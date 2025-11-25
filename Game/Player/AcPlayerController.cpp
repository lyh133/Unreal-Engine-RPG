// Fill out your copyright notice in the Description page of Project Settings.


#include "AcPlayerController.h"
#include "AcPlayerState.h"
#include "dazAda/Game/AbilitySystem/AcAbilitySystemComponent.h"

AAcPlayerController::AAcPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

AAcPlayerState* AAcPlayerController::GetAcPlayerState() const
{
	return CastChecked<AAcPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UAbilitySystemComponent* AAcPlayerController::GetAcAbilitySystemComponent() const
{
	const AAcPlayerState* AcPS = GetAcPlayerState();
	return (AcPS ? AcPS->GetAbilitySystemComponent() : nullptr);
}

void AAcPlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AAcPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AAcPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AAcPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AAcPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AAcPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
}

void AAcPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
}

void AAcPlayerController::CleanupPlayerState()
{
	Super::CleanupPlayerState();
}

void AAcPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void AAcPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void AAcPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void AAcPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);
}

void AAcPlayerController::AddCheats(bool bForce)
{
	Super::AddCheats(bForce);
}

void AAcPlayerController::UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId)
{
	Super::UpdateForceFeedback(InputInterface, ControllerId);
}

void AAcPlayerController::UpdateHiddenComponents(const FVector& ViewLocation,
	TSet<FPrimitiveComponentId>& OutHiddenComponents)
{
	Super::UpdateHiddenComponents(ViewLocation, OutHiddenComponents);
}

void AAcPlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

/*void AAcPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UAcAbilitySystemComponent* acASC = GetAcAbilitySystemComponent())
	{
		acASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
	Super::PostProcessInput(DeltaTime, bGamePaused);
}*/
