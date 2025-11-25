// Fill out your copyright notice in the Description page of Project Settings.


#include "AcCombatAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "dazAda/Game/Characters/AcBaseCharacter.h"
#include "dazAda/Game/Components/AC_StatusEffectComponent.h"

UAcCombatAttributeSet::UAcCombatAttributeSet()
{
	//InitHealth(100.f);
	HitDirectionFrontTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Front"));
	HitDirectionBackTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Back"));
	HitDirectionRightTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Right"));
	HitDirectionLeftTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Left"));
}

void UAcCombatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health,MaxHealth,NewValue,GetHealthAttribute());
	}
	if (Attribute == GetMaxStaminaAttribute())
	{
		AdjustAttributeForMaxChange(Stamina,MaxStamina,NewValue,GetStaminaAttribute());
	}

	if (Attribute == GetMaxStanceAttribute())
	{
		AdjustAttributeForMaxChange(Stance,MaxStance,NewValue,GetStanceAttribute());
	}
	
}

bool UAcCombatAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	return Super::PreGameplayEffectExecute(Data);
}

void UAcCombatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	FGameplayTagContainer SpecAssetTags;
	Data.EffectSpec.GetAllAssetTags(SpecAssetTags);

	// Get the Target actor, which should be our owner
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	AAcBaseCharacter* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<AAcBaseCharacter>(TargetActor);
	}

	// Get the Source actor
	AActor* SourceActor = nullptr;
	AController* SourceController = nullptr;
	AAcBaseCharacter* SourceCharacter = nullptr;
	if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
		SourceController = Source->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceActor != nullptr)
		{
			if (APawn* Pawn = Cast<APawn>(SourceActor))
			{
				SourceController = Pawn->GetController();
			}
		}

		// Use the controller to find the source pawn
		if (SourceController)
		{
			SourceCharacter = Cast<AAcBaseCharacter>(SourceController->GetPawn());
		}
		else
		{
			SourceCharacter = Cast<AAcBaseCharacter>(SourceActor);
		}

		// Set the causer actor based on context if it's set
		if (Context.GetEffectCauser())
		{
			SourceActor = Context.GetEffectCauser();
		}
	}
	
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		FHitResult HitResult;
		if (Context.GetHitResult())
		{
			HitResult = *Context.GetHitResult();
		}
		const float LocalDamageDone = GetDamage();

		SetDamage(0.f);
		if (LocalDamageDone > 0.0f)
		{
			bool WasAlive = true;

			if (TargetCharacter)
			{
				WasAlive = TargetCharacter->bIsAlive;
			}
			
			if (TargetCharacter && WasAlive)
			{
				
				const float NewHealth = GetHealth() - LocalDamageDone;
				SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
				TargetCharacter->SetIsAlive();
			}
			
		}
	}
	else if(Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	else if(Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));
		if (TargetCharacter && TargetCharacter->GetStatusEffectComponent())
		{
			const float StaminaPercent = GetStamina() / GetMaxStamina();
			const bool bShouldBeTired = (StaminaPercent < 0.5f);

			// Character handles applying/removing the effect
			TargetCharacter->GetStatusEffectComponent()->HandleTiredStateChanged(bShouldBeTired);
		}
	}
	else if(Data.EvaluatedData.Attribute == GetStanceAttribute())
	{
		SetStance(FMath::Clamp(GetStance(), 0.f, GetMaxStance()));
	}else if(Data.EvaluatedData.Attribute == GetStanceDamageAttribute())
	{
		FHitResult HitResult;
		if (Context.GetHitResult())
		{
			HitResult = *Context.GetHitResult();
		}
		const float LocalStanceDamageDone = GetStanceDamage();
		SetStanceDamage(0.f);
		if (LocalStanceDamageDone > 0.0f)
		{
			bool WasAlive = true;

			if (TargetCharacter)
			{
				WasAlive = TargetCharacter->bIsAlive;
			}
			if (TargetCharacter && WasAlive)
			{
				const float NewStance = GetStance() - LocalStanceDamageDone;
				SetStance(FMath::Clamp(NewStance, 0.0f, GetMaxStance()));

				const FHitResult* Hit = Data.EffectSpec.GetContext().GetHitResult();
				
				if (Hit)
				{

					
					/*EACHitReactDirection HitDirection = TargetCharacter->GetHitReactDirection(Data.EffectSpec.GetContext().GetHitResult()->Location);
					/*FString DirectionName = UEnum::GetValueAsString(HitDirection);
					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow,
						FString::Printf(TEXT("Hit Direction: %s"), *DirectionName));#1#
					
					TargetCharacter->PlayHitReact(HitDirection, SourceCharacter, LocalStanceDamageDone);*/

				}
			}
		}
	}

	//todo:
}

void UAcCombatAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
	const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}
