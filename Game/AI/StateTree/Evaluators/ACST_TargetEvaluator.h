#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeNodeBase.h"
#include "StateTreeLinker.h"
#include "Kismet/GameplayStatics.h"
#include "dazAda/Game/Characters/AcBaseCharacter.h"
#include "dazAda/Game/Characters/AcNPC.h"
#include "dazAda/Game/AI/AC_AIController.h"
#include "ACST_TargetEvaluator.generated.h"

UCLASS(Blueprintable,
    meta = (DisplayName = "Player Evaluator",
        category = "PGAS",
        ShortTooltip = "Evaluates the player.",
        Tooltip = "Monitors the player.",
        Icon = "Check_Circle")
)
class DAZADA_API UACST_TargetEvaluator : public UStateTreeEvaluatorBlueprintBase
{
    GENERATED_BODY()

public:
    /** Cache the context actor and reset health. */
    virtual void TreeStart(FStateTreeExecutionContext& Context) override
    {
        //
    }

    /** Sample health each tick and write to CurrentHealth. */
    virtual void Tick(FStateTreeExecutionContext& Context, float DeltaTime) override
    {
        if (!FocusedTarget)
        {
            SetDefaultValues(Context);
            return;
        }
        UWorld* World = Character->GetWorld();
        if (!World)
        {
            SetDefaultValues(Context);
            return;
        }


        // Calculate the distance to the player
        DistanceToTarget = FVector::Dist(Character->GetActorLocation(), FocusedTarget->GetActorLocation());

        // Check player information
        if (AAcBaseCharacter* targetCharacter = Cast<AAcBaseCharacter>(FocusedTarget))
        {
            /*TargetCharacterLevel = targetCharacter->GetCharacterLevel(); // Get the player character's level
            IsTargetAttacking = targetCharacter->IsAttacking(); // Check if the player character is attacking
            IsTargetDead = targetCharacter->IsDead(); // Check if the player character is dead*/
            
            //todo
            DistanceToTarget = FVector::Dist(Character->GetActorLocation(),FocusedTarget->GetActorLocation());
        }
    }

    /** Clear cached pointers. */
    virtual void TreeStop(FStateTreeExecutionContext& Context) override
    {
        SetDefaultValues(Context);
    }

    /**
     * Set default values for the instance data
     * @param Context The execution context for the state tree.
    */
    void SetDefaultValues(FStateTreeExecutionContext& Context)
    {
        DistanceToTarget = 0.f;
        TargetCharacterLevel = 1;
        IsTargetAttacking = false;
        IsTargetDead = false;
    }

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
            "PGAS_STEHealth",
            "HealthEvaluatorDesc",
            "Monitors the Player"
        );
        //return NSLOCTEXT("PGAS_STEPlayer", "PlayerEvaluatorDesc", "Monitors the Player");
    };

#endif
    
    /** The owning Enemy Character (set at TreeStart) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<AAcNPC> Character;
    
    /** Pointer to the bound controller */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Context, meta = (AllowPrivateAccess = "true"))
    TWeakObjectPtr<AAIController> AIController;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
    TObjectPtr<AActor> FocusedTarget = nullptr;
    
    /** Distance from ContextActor to the Player Pawn */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Output, meta = (AllowPrivateAccess = "true"))
    float DistanceToTarget = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Output)
    int32 TargetCharacterLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Output)
    bool IsTargetAttacking = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Output)
    bool IsTargetDead = false;
};

