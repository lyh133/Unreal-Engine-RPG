// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayEffectTypes.h"
#include "Animation/AnimInstance.h"
#include "LovelaceAnimInstance.generated.h"


class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class DAZADA_API ULovelaceAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	ULovelaceAnimInstance(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);
	
protected:

	#if WITH_EDITOR
		virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
	#endif // WITH_EDITOR

	
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;


	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float GroundDistance = -1.0f;
};
