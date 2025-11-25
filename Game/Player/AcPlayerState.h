// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "AcPlayerState.generated.h"

/**
 * 
 */
class UAcAbilitySystemComponent;
class UAcCombatAttributeSet;
UCLASS()
class DAZADA_API AAcPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

	
public: 
	AAcPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UAcCombatAttributeSet* GetCombatAttributeSet() const;
private:


protected:
	UPROPERTY()
	TObjectPtr<UAcCombatAttributeSet> CombatAttributeSet;

	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "Arcane|PlayerState")
	TObjectPtr<UAcAbilitySystemComponent> AbilitySystemComponent;
};
