#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "Delegates/Delegate.h" 

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeEditorPropertyBindings.h"
#include "StateTreePropertyBindings.h"

#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimTypes.h"                  
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "dazAda/Game/Characters/AcNPC.h"
#include "dazAda/Game/AI/AC_AIController.h"


#include "ACST_PlayMontageTask.generated.h"


 /**
  * A StateTree task that plays an animation montage on the context actor.
  */
UCLASS(Blueprintable, BlueprintType, category = "ACST",
    meta = (DisplayName = "Play Montage",
        Description = "A task that plays an animation montage on the context actor.",
        ToolTip = "Play an animation montage on the context actor",
        Keywords = "Animation,Montage,Task")
)
class DAZADA_API UACST_PlayMontageTask : public UStateTreeTaskBlueprintBase
{
    GENERATED_BODY()
public:
    /**
     * Functions
    */

    // Constructor
    UACST_PlayMontageTask(const FObjectInitializer& ObjectInitializer)
        : Super(ObjectInitializer)
    {
        bShouldCallTick = true; // Enables ticking every frame
    }

    /*
     * Properties
    */

    /** The enemy character this task is associated with. */
    UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context))
    TObjectPtr<AAcNPC> Actor;

    /** The AI controller managing the enemy character. */
    UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context))
    TObjectPtr<AAC_AIController> AIController;

    /** The montage to play. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task", meta = (RequiredAssetDataTags = "AnimMontage"))
    TObjectPtr<UAnimMontage> Montage = nullptr;

    /** Play rate for the montage. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task", meta = (ClampMin = "0.01", ClampMax = "10.0"))
    float PlayRate = 1.0f;

#if WITH_EDITOR
    virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override
    {
        // Get montage name and play rate
        if (!Montage)
        {
            return NSLOCTEXT("ACST", "PlayMontageTask_NoMontage", "Play Montage Task (No Montage Set)");
        }

        return FText::Format(
            NSLOCTEXT("ACST", "PlayMontageTaskDesc",
                "Play montage {0} at a speed of {1}."),
            FText::FromString(Montage->GetName()),
            FText::AsNumber(PlayRate)
        );
    }
#endif

protected:
    /**
     * Functions
    */

    /**
     * This function sets the character's MaxWalkSpeed to the TargetMaxWalkSpeed when entering the state.
     * @param Context The execution context for the state tree.
     * @param Transition The transition result containing information about the state change.
     */
    EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
    {        
        if (!ensure(Actor && Montage))
        {
            return EStateTreeRunStatus::Failed;
        }

        if (USkeletalMeshComponent* Mesh = Actor->GetMesh())
        {
            if (UAnimInstance* AnimInst = Mesh->GetAnimInstance())
            {
                // Add montage notify events.
                if (!AnimInst->OnPlayMontageNotifyBegin.IsAlreadyBound(this, &UACST_PlayMontageTask::HandleMontageNotifyBegin))
                {
                    AnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &UACST_PlayMontageTask::HandleMontageNotifyBegin);
                }

                // Add montage events.
                if (!AnimInst->OnMontageStarted.IsAlreadyBound(this, &UACST_PlayMontageTask::HandleMontageStarted))
                {
                    AnimInst->OnMontageStarted.AddDynamic(this, &UACST_PlayMontageTask::HandleMontageStarted);
                }
                if (!AnimInst->OnMontageEnded.IsAlreadyBound(this, &UACST_PlayMontageTask::HandleMontageEnded))
                {
                    AnimInst->OnMontageEnded.AddDynamic(this, &UACST_PlayMontageTask::HandleMontageEnded);
                }

                // Play the montage on the character's animation instance
                AnimInst->Montage_Play(Montage, PlayRate);
            }
        }

        // Keep running until the montage completes
        return EStateTreeRunStatus::Running;
    }

    /**
     * Called when state ticks
     * @param Context The execution context for the state tree
     * @param CompletionStatus The status of the completed state (Succeeded/Failed)
     */
    EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
    {
        // Check if montage has finished
        if (bMontageFinished)
        {
            return EStateTreeRunStatus::Succeeded;
        }

        // If we reach here, the montage is playing
        return EStateTreeRunStatus::Running;
    }

    /**
     * Called when exiting the state
     * @param Context The execution context for the state tree
     * @param Transition The transition that triggered this state
     */
    void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
    {

        if (Actor)
        {
            if (USkeletalMeshComponent* Mesh = Actor->GetMesh())
            {
                if (UAnimInstance* AnimInst = Mesh->GetAnimInstance())
                {
                    // Unbind safely
                    if (AnimInst->OnPlayMontageNotifyBegin.IsAlreadyBound(this, &UACST_PlayMontageTask::HandleMontageNotifyBegin))
                    {
                        AnimInst->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UACST_PlayMontageTask::HandleMontageNotifyBegin);
                    }
                    if (AnimInst->OnMontageStarted.IsAlreadyBound(this, &UACST_PlayMontageTask::HandleMontageStarted))
                    {
                        AnimInst->OnMontageStarted.RemoveDynamic(this, &UACST_PlayMontageTask::HandleMontageStarted);
                    }
                    if (AnimInst->OnMontageEnded.IsAlreadyBound(this, &UACST_PlayMontageTask::HandleMontageEnded))
                    {
                        AnimInst->OnMontageEnded.RemoveDynamic(this, &UACST_PlayMontageTask::HandleMontageEnded);
                    }

                    // Stop the montage if still playing
                    if (AnimInst->Montage_IsPlaying(Montage))
                    {
                        AnimInst->Montage_Stop(0.2f, Montage); // Stop the montage with a blend-out time of 0.2 seconds
                    }
                }
            }
        }
    }

private:
    /*
     * Properties
    */

    // Indicates whether the montage has finished playing
    bool bMontageFinished = false;

    /*
     * Functions
    */

    // Called when the montage starts playing
    UFUNCTION()
    void HandleMontageStarted(UAnimMontage* TheMontage)
    {

    }

    // Called when the montage ends playing
    UFUNCTION()
    void HandleMontageEnded(UAnimMontage* TheMontage, bool bInterrupted)
    {


        bMontageFinished = true; // Mark montage as finished.
    }

    // Called when a montage notify begins
    UFUNCTION()
    void HandleMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload)
    {
        // Mirror your struct's OnNotify() here, or forward to shared logic
        
    }
};
