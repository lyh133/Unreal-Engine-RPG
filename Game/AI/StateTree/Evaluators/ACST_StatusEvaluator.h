#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeEvaluatorBlueprintBase.h"
#include "GameplayTagContainer.h"
#include "dazAda/Game/Characters/AcNPC.h"
#include "ACST_StatusEvaluator.generated.h"

UCLASS(Blueprintable,
    meta = (DisplayName = "NPC Status Evaluator",
        category = "ACST",
        ShortTooltip = "Monitors fixed NPC status tags.",
        Tooltip = "Evaluates NPC status tags such as Stun, Debilitated, Tired, and Dead.",
        Icon = "Check_Circle"))
class DAZADA_API UACST_StatusEvaluator : public UStateTreeEvaluatorBlueprintBase
{
    GENERATED_BODY()

public:

    /** Initialize. */
    virtual void TreeStart(FStateTreeExecutionContext& Context) override
    {
        ResetOutputs();
    }

    /** Check gameplay tags each tick. */
    virtual void Tick(FStateTreeExecutionContext& Context, float DeltaTime) override
    {
        if (!NPC)
        {
            ResetOutputs();
            UE_LOG(LogTemp, Warning, TEXT("StatusEvaluator: NPC reference is NULL!"));
            return;
        }
        if (!NPC->GetAbilitySystemComponent())
        {
            ResetOutputs();
            UE_LOG(LogTemp, Warning, TEXT("StatusEvaluator: NPC has no AbilitySystemComponent!"));
            return;
        }
        const FGameplayTagContainer& Tags = NPC->GetAbilitySystemComponent()->GetOwnedGameplayTags();

        // Check fixed tags
        IsDebilitated = Tags.HasTagExact(Tag_Debilitated);
        IsStunned     = Tags.HasTagExact(Tag_Stun);
        IsTired       = Tags.HasTagExact(Tag_Tired);
        IsDead        = Tags.HasTagExact(Tag_Dead);
    }

    virtual void TreeStop(FStateTreeExecutionContext& Context) override
    {
        ResetOutputs();
    }

    void ResetOutputs()
    {
        IsDebilitated = false;
        IsStunned     = false;
        IsTired       = false;
        IsDead        = false;
    }

#if WITH_EDITOR
    virtual FText GetDescription(
        const FGuid& ID,
        FStateTreeDataView InstanceDataView,
        const IStateTreeBindingLookup& BindingLookup,
        EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override
    {
        return NSLOCTEXT("ACST_StatusEvaluator", "StatusEvaluatorDesc", "Monitors NPC status gameplay tags");
    }
#endif

public:

    // --------------------------------------------------
    // Context (NPC)
    // --------------------------------------------------
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Context, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<AAcNPC> NPC;

    // --------------------------------------------------
    // Fixed tags (auto-fill via defaults)
    // --------------------------------------------------
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags)
    FGameplayTag Tag_Debilitated = FGameplayTag::RequestGameplayTag(FName("State.Debuff.Debilitated"));

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags)
    FGameplayTag Tag_Stun = FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun"));

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags)
    FGameplayTag Tag_Tired = FGameplayTag::RequestGameplayTag(FName("State.Debuff.Tired"));

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Tags)
    FGameplayTag Tag_Dead = FGameplayTag::RequestGameplayTag(FName("State.Dead"));

    // --------------------------------------------------
    // Outputs for StateTree
    // --------------------------------------------------
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Output)
    bool IsDebilitated = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Output)
    bool IsStunned = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Output)
    bool IsTired = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Output)
    bool IsDead = false;
};