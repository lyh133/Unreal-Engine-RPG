// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "dazAda/Game/AbilitySystem/AcGameplayAbility.h"
#include "AnimatedAbility.generated.h"

/**
 * 
 */
UCLASS()
class DAZADA_API UAnimatedAbility : public UAcGameplayAbility
{
	GENERATED_BODY()
	
protected:
	float PlayAbilityMontage(UAnimMontage* Montage);
	
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
public:
	UAnimatedAbility();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* AnimMontage;
};
