// Fill out your copyright notice in the Description page of Project Settings.


#include "ACGA_QuickStep.h"
#include "dazAda/Game/Characters/AcBaseCharacter.h"
UACGA_QuickStep::UACGA_QuickStep()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FGameplayTag Ability2Tag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Ability2"));
	AbilityTags.AddTag(Ability2Tag);
	ActivationOwnedTags.AddTag(Ability2Tag);
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill")));
}

void UACGA_QuickStep::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
	
	// Get player controller
	APlayerController* PC = Cast<APlayerController>(CachedCharacter->GetController());
	if (!PC)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	// Check keyboard input
	bool bW = PC->IsInputKeyDown(EKeys::W);
	bool bA = PC->IsInputKeyDown(EKeys::A);
	bool bS = PC->IsInputKeyDown(EKeys::S);
	bool bD = PC->IsInputKeyDown(EKeys::D);
	
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("Keys - W:%d A:%d S:%d D:%d"), bW, bA, bS, bD));
	
	UAnimMontage* MontageToPlay = nullptr;
	
	// 8-way directional logic
	if (bW && bD) // Forward-Right
	{
		MontageToPlay = DodgeForwardRightMontage;
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("FORWARD-RIGHT"));
	}
	else if (bW && bA) // Forward-Left
	{
		MontageToPlay = DodgeForwardLeftMontage;
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("FORWARD-LEFT"));
	}
	else if (bS && bD) // Back-Right
	{
		MontageToPlay = DodgeBackRightMontage;
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("BACK-RIGHT"));
	}
	else if (bS && bA) // Back-Left
	{
		MontageToPlay = DodgeBackLeftMontage;
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("BACK-LEFT"));
	}
	else if (bW) // Forward
	{
		MontageToPlay = DodgeForwardMontage;
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("FORWARD"));
	}
	else if (bS) // Back
	{
		MontageToPlay = DodgeBackMontage;
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("BACK"));
	}
	else if (bD) // Right
	{
		MontageToPlay = DodgeRightMontage;
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("RIGHT"));
	}
	else if (bA) // Left
	{
		MontageToPlay = DodgeLeftMontage;
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("LEFT"));
	}
	else // No input, default forward
	{
		MontageToPlay = DodgeForwardMontage;
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("FORWARD (Default)"));
	}
	
	if (MontageToPlay)
	{
		PlayAbilityMontage(MontageToPlay);
	}
}

float UACGA_QuickStep::PlayAbilityMontage(UAnimMontage* Montage)
{
	if (!CachedCharacter || !Montage) return 0.f;

	UAnimInstance* AnimInstance = CachedCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return 0.f;

	// Use ASC helper instead of raw Montage_Play
	float Duration = AnimInstance->Montage_Play(Montage, 1.0);

	if (Duration > 0.f)
	{
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(this, &UACGA_QuickStep::OnMontageEnded);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, Montage);
	}

	return Duration;
}

void UACGA_QuickStep::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{

		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}