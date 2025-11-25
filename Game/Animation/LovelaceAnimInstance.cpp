// Fill out your copyright notice in the Description page of Project Settings.


#include "LovelaceAnimInstance.h"
#include "AbilitySystemGlobals.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif
//todo write character file


ULovelaceAnimInstance::ULovelaceAnimInstance(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}



//todo
void ULovelaceAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);
	GameplayTagPropertyMap.Initialize(this, ASC);
}

void ULovelaceAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}
//todo
void ULovelaceAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}



#if WITH_EDITOR
EDataValidationResult ULovelaceAnimInstance::IsDataValid(FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);

	GameplayTagPropertyMap.IsDataValid(this, Context);

	return ((Context.GetNumErrors() > 0) ? EDataValidationResult::Invalid : EDataValidationResult::Valid);
}
#endif // WITH_EDITOR