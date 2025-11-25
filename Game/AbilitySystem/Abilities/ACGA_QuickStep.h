// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "dazAda/Game/AbilitySystem/AcGameplayAbility.h"
#include "ACGA_QuickStep.generated.h"

/**
 * 
 */
UCLASS()
class DAZADA_API UACGA_QuickStep : public UAcGameplayAbility
{
	GENERATED_BODY()
protected:
	float PlayAbilityMontage(UAnimMontage* Montage);
	
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
public:
	UACGA_QuickStep();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;


	/*UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* DodgeForwardMontage;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* DodgeRightMontage;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* DodgeLeftMontage;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* DodgeBackMontage;*/


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuickStep|Montages")
	UAnimMontage* DodgeForwardMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuickStep|Montages")
	UAnimMontage* DodgeForwardRightMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuickStep|Montages")
	UAnimMontage* DodgeRightMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuickStep|Montages")
	UAnimMontage* DodgeBackRightMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuickStep|Montages")
	UAnimMontage* DodgeBackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuickStep|Montages")
	UAnimMontage* DodgeBackLeftMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuickStep|Montages")
	UAnimMontage* DodgeLeftMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QuickStep|Montages")
	UAnimMontage* DodgeForwardLeftMontage;
	
};

