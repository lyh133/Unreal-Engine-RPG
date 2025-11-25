// Fill out your copyright notice in the Description page of Project Settings.


#include "ACST_PerceptionEvaluator.h"
#include "dazAda/Game/Characters/AcNPC.h"
#include "dazAda/Game/Characters/AcBaseCharacter.h"
#include "dazAda/Game/AI/AC_AIController.h"
#include "StateTreeExecutionContext.h"

/**
  * Initializes the evaluator and binds to the AIController's perception events.
  * This is called when the StateTree starts.
  * @param Context The execution context for the StateTree.
 */
void UACST_PerceptionEvaluator::TreeStart(FStateTreeExecutionContext& Context)
{
    Super::TreeStart(Context);

    // Set our context ai controller (owner of the StateTree)
    AAC_AIController* aiController = Cast<AAC_AIController>(Context.GetOwner());
    if (!aiController)
    {
        UE_LOG(LogTemp, Error, TEXT("UPGAS_STE_Perception: StateTree owner is not an AAC_AIController!"));
        return;
    }
    AIController = aiController;

    // Bind to controller's delegates (avoid duplicate binds)
    AIController->OnSightStimulusDetected.AddDynamic(this, &UACST_PerceptionEvaluator::HandleSightStimulus);
    AIController->OnSightStimulusForgotten.AddDynamic(this, &UACST_PerceptionEvaluator::HandleSightStimulusForgotten);
    AIController->OnHearingStimulusDetected.AddDynamic(this, &UACST_PerceptionEvaluator::HandleHearingStimulus);
    AIController->OnHearingStimulusForgotten.AddDynamic(this, &UACST_PerceptionEvaluator::HandleHearingStimulusForgotten);
    AIController->OnDamageStimulusDetected.AddDynamic(this, &UACST_PerceptionEvaluator::HandleDamageStimulus);

    // Set the initial state
    AIController->CurrentStateTreeState = EStateTreeEvent::Unknown; // Update the AIController's state tree state

    // Get the Pawn from the AI Controller
    APawn* OwnerPawn = AIController->GetPawn();
    if (!OwnerPawn)
    {
        UE_LOG(LogTemp, Error, TEXT("UPGAS_STE_Perception: AIController has no Pawn!"));
        return;
    }

    // Set our context character
    Character = Cast<AAcNPC>(OwnerPawn);
    if (!Character)
    {
        UE_LOG(LogTemp, Warning, TEXT("UPGAS_STE_Perception: Pawn is not an APGAS_EnemyCharacter! Class is: %s"), *OwnerPawn->GetClass()->GetName());
        return;
    }

    // Store StateTreeComp reference
    StateTreeComp = AIController->FindComponentByClass<UStateTreeAIComponent>();
    if (!StateTreeComp)
    {
        UE_LOG(LogTemp, Error, TEXT("UPGAS_STE_Perception: No UStateTreeAIComponent found on AIController!"));
        return;
    }
}

/**
 * Cleans up the evaluator and unbinds from the AIController's perception events.
 * This is called when the StateTree stops.
 * @param Context The execution context for the StateTree.
*/
void UACST_PerceptionEvaluator::TreeStop(FStateTreeExecutionContext& Context)
{
    Super::TreeStop(Context);

    if (AIController.IsValid())
    {
        AIController->OnSightStimulusDetected.RemoveDynamic(this, &UACST_PerceptionEvaluator::HandleSightStimulus);
        AIController->OnSightStimulusForgotten.RemoveDynamic(this, &UACST_PerceptionEvaluator::HandleSightStimulusForgotten);
        AIController->OnHearingStimulusDetected.RemoveDynamic(this, &UACST_PerceptionEvaluator::HandleHearingStimulus);
        AIController->OnHearingStimulusForgotten.RemoveDynamic(this, &UACST_PerceptionEvaluator::HandleHearingStimulusForgotten);
        AIController->OnDamageStimulusDetected.RemoveDynamic(this, &UACST_PerceptionEvaluator::HandleDamageStimulus);

        AIController->CurrentStateTreeState = EStateTreeEvent::Unknown; // Update the AIController's state tree state
    }

    StateTreeComp = nullptr; // Clear the StateTree component reference
    AcquiredTarget = nullptr; // Clear the acquired target
}

// Broadcast handlers for perception events
void UACST_PerceptionEvaluator::HandleDamageStimulus(AActor* Actor, const FAIStimulus& Stimulus)
{

    AAcBaseCharacter* BaseChar = Cast<AAcBaseCharacter>(Actor);

    if (!BaseChar) return;
    if (AIController->CurrentStateTreeState != EStateTreeEvent::Attack)
    {
        AcquiredTarget = BaseChar;
        if (AIController.IsValid())
            AIController->AcquiredTarget = Actor; // Clear the AIController's target
        OnDamageStimulus(Actor, Stimulus);

        SendEvent(EStateTreeEvent::Attack); // Send event to StateTree
        AIController->CurrentStateTreeState = EStateTreeEvent::Attack; // Update the AIController's state tree state
    }
}

void UACST_PerceptionEvaluator::HandleSightStimulus(AActor* Actor, const FAIStimulus& Stimulus)
{

    AAcBaseCharacter* BaseChar = Cast<AAcBaseCharacter>(Actor);

    if (!BaseChar) return;
    
    if (AIController->CurrentStateTreeState != EStateTreeEvent::Attack)
    {
        AcquiredTarget = BaseChar;
        if (AIController.IsValid())
            AIController->AcquiredTarget = Actor; // Clear the AIController's target
        OnSightStimulus(Actor, Stimulus);

        SendEvent(EStateTreeEvent::Attack); // Send event to StateTree
        AIController->CurrentStateTreeState = EStateTreeEvent::Attack; // Update the AIController's state tree state
    }
}

void UACST_PerceptionEvaluator::HandleSightStimulusForgotten(AActor* Actor)
{
    UE_LOG(LogTemp, Log, TEXT("UPGAS_STE_Perception: Sight stimulus forgotten for actor: %s"), *GetNameSafe(Actor));
    AcquiredTarget = nullptr;
    if (AIController.IsValid())
        AIController->AcquiredTarget = nullptr; // Clear the AIController's target
    AIController->CurrentStateTreeState = EStateTreeEvent::Unknown; // Update the AIController's state tree state
    OnSightStimulusForgotten(Actor);
}

void UACST_PerceptionEvaluator::HandleHearingStimulus(AActor* Actor, const FAIStimulus& Stimulus)
{
    AAcBaseCharacter* BaseChar = Cast<AAcBaseCharacter>(Actor);
    if (!BaseChar) return;
    if (AIController->CurrentStateTreeState != EStateTreeEvent::Attack)
    {
        AcquiredTarget = BaseChar;
        if (AIController.IsValid())
            AIController->AcquiredTarget = Actor; // Clear the AIController's target
        OnHearingStimulus(Actor, Stimulus);
        SendEvent(EStateTreeEvent::Attack); // Send event to StateTree
        AIController->CurrentStateTreeState = EStateTreeEvent::Attack; // Update the AIController's state tree state
    }
}

void UACST_PerceptionEvaluator::HandleHearingStimulusForgotten(AActor* Actor)
{
    UE_LOG(LogTemp, Log, TEXT("UPGAS_STE_Perception: Hearing stimulus forgotten for actor: %s"), *GetNameSafe(Actor));
    AcquiredTarget = nullptr;
    if (AIController.IsValid())
        AIController->AcquiredTarget = nullptr; // Clear the AIController's target
    AIController->CurrentStateTreeState = EStateTreeEvent::Unknown; // Update the AIController's state tree state
    OnHearingStimulusForgotten(Actor);
}