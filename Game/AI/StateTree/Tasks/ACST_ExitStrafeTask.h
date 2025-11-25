#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "GameFramework/Actor.h"
#include "dazAda/Game/Characters/AcNPC.h"
#include "dazAda/Game/AI/AC_AIController.h"
#include "ACST_ExitStrafeTask.generated.h"

/**
 * Instance data for the Enter Strafe task.
 */
USTRUCT(BlueprintType, Category = "ACST|Combat",
    meta = (DisplayName = "Enter Strafe Instance Data",
        Description = "Contains instance data for the Enter Strafe task.",
        ToolTip = "Instance data used when entering a strafe behavior."))
struct FExitStrafeTaskInstanceData
{
    GENERATED_BODY()

    /** The controlled NPC. */
    UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context))
    TObjectPtr<AAcNPC> Actor;

    /** The AI Controller. */
    UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context))
    TObjectPtr<AAC_AIController> AIController;
    
};


/**
 * Enter Strafe Task
 * Called when AI transitions into the "Strafe" combat behavior.
 */
USTRUCT(Category = "ACST",
    meta = (DisplayName = "exit Strafe",
        Description = "StateTree task to initialize strafe behavior.",
        ToolTip = "Initial setup task when entering a strafe state.",
        Keywords = "Combat, Strafe, AI, StateTree"))
struct DAZADA_API FACST_ExitStrafeTask : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FExitStrafeTaskInstanceData;

    virtual const UStruct* GetInstanceDataType() const override
    {
        return FExitStrafeTaskInstanceData::StaticStruct();
    }

    /*
     * Functions
     */

    /** Called when entering the Strafe state. */
    virtual EStateTreeRunStatus EnterState(
        FStateTreeExecutionContext& Context,
        const FStateTreeTransitionResult& Transition) const override
    {

        // Get the instance data for this task
        FExitStrafeTaskInstanceData& Data = Context.GetInstanceData(*this);

        // Safety checks
        if (!Data.Actor)
        {
            UE_LOG(LogTemp, Warning, TEXT("FExitStrafeTaskInstanceData: Actor is null."));
            return EStateTreeRunStatus::Failed;
        }
        Data.Actor->lockCamera = false;
        Data.Actor->bUseControllerRotationYaw = false;
        
        return EStateTreeRunStatus::Succeeded;
    }


};