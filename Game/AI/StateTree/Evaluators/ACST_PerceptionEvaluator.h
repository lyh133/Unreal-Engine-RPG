// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "Perception/AIPerceptionTypes.h"
#include "Components/StateTreeAIComponent.h"
#include "dazAda/dazAda.h"
#include "ACST_PerceptionEvaluator.generated.h"


class AAC_AIController;
class AAcNPC;
class AAcBaseCharacter;


UCLASS(Blueprintable, 
    meta = (
        DisplayName = "AI Perception Evaluator",
        category = "ACST",
        ShortTooltip = "Evaluates the AI perception of the context actor.",
        Tooltip = "Monitors the AI perception GAS attribute of the context actor."
    )
)
class DAZADA_API UACST_PerceptionEvaluator : public UStateTreeEvaluatorBlueprintBase
{
    GENERATED_BODY()

public:
    /*
     * Functions
    */

    /**
     * Initializes the evaluator and binds to the AIController's perception events.
     * This is called when the StateTree starts.
     * @param Context The execution context for the StateTree.
     */
    virtual void TreeStart(FStateTreeExecutionContext& Context) override;

    /**
     * Cleans up the evaluator and unbinds from the AIController's perception events.
     * This is called when the StateTree stops.
     * @param Context The execution context for the StateTree.
     */
    virtual void TreeStop(FStateTreeExecutionContext& Context) override;

#if WITH_EDITOR

    /**
     * Gets the description of the condition for editor display.
     * @param ID The unique identifier for the node.
     * @param InstanceDataView The data view for the instance.
     * @param BindingLookup The binding lookup for the state tree.
     * @param Formatting The formatting options for the description.
     * @return The formatted description text.
    */
    virtual FText GetDescription(
        const FGuid& ID,
        FStateTreeDataView InstanceDataView,
        const IStateTreeBindingLookup& BindingLookup,
        EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const {
        return NSLOCTEXT(
            "UACST_PerceptionEvaluator",
            "PerceptionDesc",
            "Monitors the Context Actor's AI perceptions"
        );
    };

#endif

    /*
     * Properties
    */

    /** The owning Enemy Character (set at TreeStart) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<AAcNPC> Character;

    /** Pointer to the bound controller */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context, meta = (AllowPrivateAccess = "true"))
    TWeakObjectPtr<AAC_AIController> AIController;

    /**
     * The current acquired target from perception events
     * Used in State Trees to determine the current focus of the AI and is easily bindable.
    */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Output, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<AAcBaseCharacter> AcquiredTarget = nullptr;

protected:
    /*
     * Functions
    */

    /** Called when sight stimulus is detected */
    UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
    void OnSightStimulus(AActor* Actor, const FAIStimulus& Stimulus);

    /** Called when sight stimulus is forgotten */
    UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
    void OnSightStimulusForgotten(AActor* Actor);

    /** Called when hearing stimulus is detected */
    UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
    void OnHearingStimulus(AActor* Actor, const FAIStimulus& Stimulus);

    /** Called when hearing stimulus is forgotten */
    UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
    void OnHearingStimulusForgotten(AActor* Actor);

    /** Called when damage stimulus is detected */
    UFUNCTION(BlueprintImplementableEvent, Category = "Perception")
    void OnDamageStimulus(AActor* Actor, const FAIStimulus& Stimulus);

private:
    /*
     * Functions
    */

    /** Native C++ handlers for delegate binding */
    UFUNCTION()
    void HandleSightStimulus(AActor* Actor, const FAIStimulus& Stimulus);

    UFUNCTION()
    void HandleSightStimulusForgotten(AActor* Actor);

    UFUNCTION()
    void HandleHearingStimulus(AActor* Actor, const FAIStimulus& Stimulus);

    UFUNCTION()
    void HandleHearingStimulusForgotten(AActor* Actor);

    UFUNCTION()
    void HandleDamageStimulus(AActor* Actor, const FAIStimulus& Stimulus);

    /** Sends a StateTree event with the specified tag */
    void SendEvent(const EStateTreeEvent event)
    {
        // Get event tag based on enum value
        FGameplayTag EventTag;
        switch (event)
        {
            case EStateTreeEvent::Idle:
                EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Event.Idle"));
                LastEvent = EStateTreeEvent::Idle;
                break;
            case EStateTreeEvent::Attack:
                EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Event.Attack"));
                LastEvent = EStateTreeEvent::Attack;
                break;
            case EStateTreeEvent::Patrol:
                EventTag = FGameplayTag::RequestGameplayTag(FName("StateTree.Event.Patrol"));
                LastEvent = EStateTreeEvent::Patrol;
                break;
            default:
                return; // Ignore unknown events
        }

        // Send the event to the StateTree component if it exists
        if (AIController.IsValid() && StateTreeComp)
        {
            FStateTreeEvent Event(EventTag);
            StateTreeComp->SendStateTreeEvent(Event);
        }
    }

    /*
     * Properties
    */

    UPROPERTY()
    TObjectPtr<UStateTreeAIComponent> StateTreeComp;
    
    /** Last event sent to the StateTree */
    EStateTreeEvent LastEvent = EStateTreeEvent::Idle; // Track the last event sent to the StateTree
};