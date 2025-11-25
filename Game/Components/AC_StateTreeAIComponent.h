// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StateTreeAIComponent.h"
#include "GameplayTagContainer.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeInstanceData.h"
#include "AC_StateTreeAIComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Arcane), meta = (BlueprintSpawnableComponent, DisplayName = "Arcane State Tree AI Component"))
class DAZADA_API UAC_StateTreeAIComponent : public UStateTreeAIComponent
{
	GENERATED_BODY()
	
public:
	/*
	 * Functions
	*/

	// Constructor
	UAC_StateTreeAIComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Called when the component is initialized */
	virtual void InitializeComponent() override;

	/** Validates the State Tree reference */
	virtual void ValidateStateTreeReference() override;

	/**
	 * Sets the State Tree reference for this component and starts the logic.
	 * @param StateTreeReference The State Tree reference to set.
	*/
	UFUNCTION(BlueprintCallable, Category = "StateTree", meta = (AutoCreateRefTerm = "StateTreeReference", DisplayName = "Start State Tree"))
	void StartStateTree(UStateTree* StateTree);

	/**
	 * Stops the currently running State Tree logic.
	 */
	UFUNCTION(BlueprintCallable, Category = "StateTree", meta = (DisplayName = "Stop State Tree"))
	void StopStateTree();

	/**
	 * Sends an event to the State Tree.
	 * @param EventTag The gameplay tag representing the event to send.
	*/
	UFUNCTION(BlueprintCallable, Category = "StateTree", meta = (AutoCreateRefTerm = "EventTag", DisplayName = "Send Event"))
	void SendEvent(const FGameplayTag& EventTag);

private:

	/*
	 * Properties
	*/

	/** The last event tag sent to the State Tree */
	FGameplayTag LastEventTag = FGameplayTag::EmptyTag;
};
 
