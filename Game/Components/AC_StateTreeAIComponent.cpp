// Fill out your copyright notice in the Description page of Project Settings.


#include "dazAda/Game/Components/AC_StateTreeAIComponent.h"
#include "StateTreeSchema.h"
UAC_StateTreeAIComponent::UAC_StateTreeAIComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;

    SetStartLogicAutomatically(true);  // Do not start logic automatically on BeginPlay
}

void UAC_StateTreeAIComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UAC_StateTreeAIComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

/** Called when the component is initialized */
void UAC_StateTreeAIComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

/** Validates the State Tree reference */
void UAC_StateTreeAIComponent::ValidateStateTreeReference()
{
    Super::ValidateStateTreeReference();
}

/**
 * Sets the State Tree reference for this component and starts the logic.
 * @param StateTreeReference The State Tree reference to set.
*/
void UAC_StateTreeAIComponent::StartStateTree(UStateTree* StateTree)
{
    if (StateTree)
    {
        if (IsRunning())
        {
            Cleanup();
        }

        StateTreeRef.SetStateTree(StateTree);
        Super::SetStateTree(StateTree);
        StartLogic();
    }
}

/**
 * Stops the currently running State Tree logic.
 */
void UAC_StateTreeAIComponent::StopStateTree()
{
    if (IsRunning())
    {
        Cleanup();
    }
    
    StopLogic(FString("Stopped by Code"));
    Super::SetStateTree(nullptr);
    StateTreeRef.SetStateTree(nullptr);
}

/**
 * Sends an event to the State Tree.
 * @param EventTag The gameplay tag representing the event to send.
*/
void UAC_StateTreeAIComponent::SendEvent(const FGameplayTag& EventTag)
{
    if (IsRunning() && LastEventTag != EventTag)
    {
        LastEventTag = EventTag; // Update the last event tag to prevent duplicates
        FStateTreeEvent Event = FStateTreeEvent(EventTag);
        SendStateTreeEvent(Event); // Send the event to the StateTree execution context
        // UE_LOG(LogTemp, Warning, TEXT("UPGAS_StateTreeAIComponent::SendEvent - Event '%s' sent."), *EventTag.GetTagName().ToString());
    }
    else
    {
        // UE_LOG(LogTemp, Warning, TEXT("UPGAS_StateTreeAIComponent::SendEvent - StateTree execution context is not valid. Event '%s' not sent."), *EventTag.GetTagName().ToString());
    }
}