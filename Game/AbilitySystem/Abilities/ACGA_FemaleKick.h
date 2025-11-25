// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "dazAda/Game/AbilitySystem/AcGameplayAbility.h"
#include "ACGA_FemaleKick.generated.h"

/**
 * 
 */
UCLASS()
class DAZADA_API UACGA_FemaleKick : public UAcGameplayAbility
{
	GENERATED_BODY()
public:
	UACGA_FemaleKick();
	
	/** Actually activate ability, do not call this directly. We'll call it from APAHeroCharacter::ActivateAbilitiesWithTags(). */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly)
	float ComboResetTime = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxComboCount = 3; 

protected:
	

	UPROPERTY()
	TSet<AActor*> AlreadyHitActors;

	UPROPERTY()
	int32 CurrentComboStep = 0;
	
	UPROPERTY()
	FTimerHandle ComboResetTimer;
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

private:
	void ResetCombo();
};
