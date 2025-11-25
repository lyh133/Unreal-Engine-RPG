// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "ACST_UpdateMovementSpeed.generated.h"


class AAcNPC;
class AAC_AIController;
/**
 * 
 */

UCLASS(Blueprintable, BlueprintType, Category = "Arcane", meta = (DisplayName = "Update Movement Speed"))
class DAZADA_API UACST_UpdateMovementSpeed : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
	
public:
	/*
	 * Properties
	*/

	/** The enemy character this task is associated with. */
	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context))
	TObjectPtr<AAcNPC> Actor;

	/** The AI controller managing the enemy character. */
	UPROPERTY(BlueprintReadOnly, Category = Context, meta = (Context))
	TObjectPtr<AAC_AIController> AIController;

	/** Desired MaxWalkSpeed to apply while this task is active. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
	float TargetMaxWalkSpeed = 600.0f;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif

protected:
	/*
	 * Functions
	*/

	/**
	 * This function sets the character's MaxWalkSpeed to the TargetMaxWalkSpeed when entering the state.
	 * @param Context The execution context for the state tree.
	 * @param Transition The transition result containing information about the state change.
	*/
	EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition);
};
