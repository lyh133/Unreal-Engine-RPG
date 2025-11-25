// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AcAttributeSet.h"
#include "AcCombatAttributeSet.generated.h"


struct FGameplayEffectModCallbackData;
/**
 * 
 */
UCLASS(BlueprintType)
class DAZADA_API UAcCombatAttributeSet : public UAcAttributeSet
{
	GENERATED_BODY()
	UAcCombatAttributeSet();
	
public:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, Damage)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, StanceDamage)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, BaseAR)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, BaseMR)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, BaseAP)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, BaseAD)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, BaseForce)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, ADRecieveMod)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, MDRecieveMod)
	
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, Health)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, MaxHealth)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, HealthRegen)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, Stamina)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, MaxStamina)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, StaminaRegen)

	

	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, Stance)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, MaxStance)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, StanceRegen)

	
	
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, JogStaminaCost)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, SprintStaminaCost)
	
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, BaseMovementSpeed)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, EffectiveMovementSpeed)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, JogSpeedBoost)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, SprintSpeedBoost)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, BaseMovementAcceleration)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, EffectiveMovementAcceleration)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, JogAccelerationBoost)
	ATTRIBUTE_ACCESSORS(UAcCombatAttributeSet, SprintAccelerationSpeedBoost)


	//combat

	// Damage is a meta attribute used by the DamageExecution to calculate final damage, which then turns into -Health
	// Temporary value that only exists on the Server. Not replicated.
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Damage;

	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData StanceDamage;
	
	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute")
	FGameplayAttributeData BaseAR;

	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute")
	FGameplayAttributeData BaseMR;

	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute")
	FGameplayAttributeData ADRecieveMod;

	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute")
	FGameplayAttributeData MDRecieveMod;

	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute")
	FGameplayAttributeData Stance;

	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute")
	FGameplayAttributeData MaxStance;

	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute")
	FGameplayAttributeData BaseAD;

	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute")
	FGameplayAttributeData BaseAP;

	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute")
	FGameplayAttributeData BaseForce;
private:

	FGameplayTag HitDirectionFrontTag;
	FGameplayTag HitDirectionBackTag;
	FGameplayTag HitDirectionRightTag;
	FGameplayTag HitDirectionLeftTag;
	
	//might replicate later
	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;
	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;
	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Stamina;
	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxStamina;

	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HealthRegen;
	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData StaminaRegen;
	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData StanceRegen;
	

	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData JogStaminaCost;
	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SprintStaminaCost;


	//movement
	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseMovementSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData EffectiveMovementSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData JogSpeedBoost;
	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SprintSpeedBoost;

	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseMovementAcceleration;
	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData EffectiveMovementAcceleration;
	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData JogAccelerationBoost;
	UPROPERTY(BlueprintReadOnly, Category = "Ability |Gameplay Attribute", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SprintAccelerationSpeedBoost;

	

	
protected:
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	// Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes.
	// (i.e. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before)
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);
};
