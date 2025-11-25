// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Components/ActorComponent.h"
#include "AcCharacterStatusComponent.generated.h"


class UAbilitySystemComponent;
class UAcCombatAttributeSet;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DAZADA_API UAcCharacterStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAcCharacterStatusComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	FTimerHandle StatusUpdateTimer;
	void UpdateCharacterStatus();
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	const UAcCombatAttributeSet* CombatAttributeSet;
};
