// Fill out your copyright notice in the Description page of Project Settings.


#include "ACDamageExecCalculation.h"
#include "AcAbilitySystemComponent.h"
#include "dazAda/Game/AbilitySystem/Attributes/AcCombatAttributeSet.h"

struct ACDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseMR);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ADRecieveMod);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MDRecieveMod);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BaseAR);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(StanceDamage);
	ACDamageStatics()
	{
		// Snapshot happens at time of GESpec creation

		// We're not capturing anything from the Source in this example, but there could be like AttackPower attributes that you might want.

		// Capture optional Damage set on the damage GE as a CalculationModifier under the ExecutionCalculation
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAcCombatAttributeSet, Damage, Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAcCombatAttributeSet, StanceDamage, Source, true);

		// Capture the Target's Armor. Don't snapshot.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAcCombatAttributeSet, BaseAR, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAcCombatAttributeSet, BaseMR, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAcCombatAttributeSet, ADRecieveMod, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAcCombatAttributeSet, MDRecieveMod, Target, false);
	}
};
static const ACDamageStatics& DamageStatics()
{
	static ACDamageStatics DStatics;
	return DStatics;
}

UACDamageExecCalculation::UACDamageExecCalculation()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().StanceDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().BaseARDef);
	RelevantAttributesToCapture.Add(DamageStatics().BaseMRDef);
	RelevantAttributesToCapture.Add(DamageStatics().ADRecieveModDef);
	RelevantAttributesToCapture.Add(DamageStatics().MDRecieveModDef);
}

void UACDamageExecCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();
	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor() : nullptr;
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float PhysicalDefense = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseARDef, EvaluationParameters, PhysicalDefense);
	float AD_recieveMod = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ADRecieveModDef, EvaluationParameters, AD_recieveMod);

	float Damage = 0.0f;
	// Capture optional damage value set on the damage GE as a CalculationModifier under the ExecutionCalculation
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, Damage);
	// Add SetByCaller damage if it exists
	Damage += FMath::Max<float>(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")),false, -1.0f), 0.0f);
	float UnmitigatedDamage = Damage * AD_recieveMod;
	

	float MitigatedDamage = (UnmitigatedDamage) * (100 / (100 + PhysicalDefense));
	if (MitigatedDamage > 0.f)
	{
		// Set the Target's damage meta attribute
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, MitigatedDamage));
	}
	UAcAbilitySystemComponent* TargetASC = Cast<UAcAbilitySystemComponent>(TargetAbilitySystemComponent);
	if (TargetASC)
	{
		UAcAbilitySystemComponent* SourceASC = Cast<UAcAbilitySystemComponent>(SourceAbilitySystemComponent);
		TargetASC->ReceiveDamage(SourceASC, UnmitigatedDamage, MitigatedDamage);
	}

	float stanceDamage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().StanceDamageDef, EvaluationParameters, stanceDamage);
	stanceDamage += FMath::Max<float>(Spec.GetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.StanceDamage")),false, -1.0f), 0.0f);
	if (stanceDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().StanceDamageProperty, EGameplayModOp::Additive, stanceDamage));
	}
	
}
