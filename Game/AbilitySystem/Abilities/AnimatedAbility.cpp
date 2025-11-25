// Fill out your copyright notice in the Description page of Project Settings.


#include "dazAda/Game/AbilitySystem/Abilities/AnimatedAbility.h"
#include "dazAda/Game/Characters/AcBaseCharacter.h"


void UAnimatedAbility::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

UAnimatedAbility::UAnimatedAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill")));
}

void UAnimatedAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	CachedCharacter = Cast<AAcBaseCharacter>(ActorInfo->AvatarActor.Get());
	
	if (!CachedCharacter)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	if (AnimMontage)
		PlayAbilityMontage(AnimMontage);
	
}
float UAnimatedAbility::PlayAbilityMontage(UAnimMontage* Montage)
{
	if (!CachedCharacter || !Montage) return 0.f;

	UAnimInstance* AnimInstance = CachedCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return 0.f;

	// Use ASC helper instead of raw Montage_Play
	float Duration = AnimInstance->Montage_Play(Montage, 1.0);

	if (Duration > 0.f)
	{
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &UAnimatedAbility::OnMontageEnded);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
	}

	return Duration;
}