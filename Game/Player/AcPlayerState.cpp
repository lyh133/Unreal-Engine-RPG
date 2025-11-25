// Fill out your copyright notice in the Description page of Project Settings.


#include "AcPlayerState.h"
#include "dazAda/Game/AbilitySystem/AcAbilitySystemComponent.h"
#include "dazAda/Game/AbilitySystem/Attributes/AcCombatAttributeSet.h"
class APlayerState;

AAcPlayerState::AAcPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAcAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	CombatAttributeSet = CreateDefaultSubobject<UAcCombatAttributeSet>("CombatAttributeSet");
}

UAbilitySystemComponent* AAcPlayerState::GetAbilitySystemComponent() const
{  
	return AbilitySystemComponent;
}

UAcCombatAttributeSet* AAcPlayerState::GetCombatAttributeSet() const
{
	return CombatAttributeSet;
}
