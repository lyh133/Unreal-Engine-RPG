// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "dazAda/Game//AbilitySystem//AcAbilitySystemComponent.h"
#include "AcAttributeSet.generated.h"

/**
 * 
 */
class AActor;
class ULyraAbilitySystemComponent;
class UObject;
class UWorld;
struct FGameplayEffectSpec;


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)




UCLASS() 
class DAZADA_API UAcAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UAcAttributeSet();
	

};
