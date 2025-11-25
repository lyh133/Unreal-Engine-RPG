// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "dazAda/dazAda.h"
#include "AcGameplayAbility.generated.h"

class AAcBaseCharacter;
/**
 * 
 */
UCLASS()
class DAZADA_API UAcGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	AAcBaseCharacter* CachedCharacter;
public:
	UAcGameplayAbility();
	// Abilities with this set will automatically activate when the input is pressed
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EACAbilityInputID AbilityInputID = EACAbilityInputID::None;

	// Value to associate an ability with an slot without tying it to an automatically activated input.
	// Passive abilities won't be tied to an input so we need a way to generically associate abilities with slots.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EACAbilityInputID AbilityID = EACAbilityInputID::None;

	// Tells an ability to activate immediately when its granted. Used for passive abilities and abilities forced on others.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
	bool ActivateAbilityOnGranted = false;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* AbilityMontage;

	void ApplyDamageEffectToActor(AActor* HitActor);
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float BasePhysicalDamage;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float BaseMagicalDamage;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float BaseForce;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float ForceMult;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float AD_Mult;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float AP_Mult;
	
	
	
};
