// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "dazAda/Game/AbilitySystem/AcGameplayAbility.h"

#include "ACGA_FemalePunch.generated.h"

class AAcBaseCharacter;
/**
 * 
 */
UCLASS()
class DAZADA_API UACGA_FemalePunch : public UAcGameplayAbility
{
	GENERATED_BODY()
public:
	UACGA_FemalePunch();
	
	/** Actually activate ability, do not call this directly. We'll call it from APAHeroCharacter::ActivateAbilitiesWithTags(). */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;


protected:
	

	UPROPERTY()
	TSet<AActor*> AlreadyHitActors;

	
	/*UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);
	
	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);*/

	
	UFUNCTION()
	void OnWeaponTraceFinished(const TArray<FHitResult>& HitResults);

	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	float PlayAbilityMontage();

};
