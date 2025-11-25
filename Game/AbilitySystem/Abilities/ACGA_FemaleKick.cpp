// Fill out your copyright notice in the Description page of Project Settings.


#include "dazAda/Game/AbilitySystem/Abilities/ACGA_FemaleKick.h"
#include "AbilitySystemComponent.h"
#include "dazAda/Game/Interface/CharacterDataInterface.h"
#include "dazAda/Game/Characters/AcBaseCharacter.h"
#include "Animation/AnimInstance.h"
#include "Abilities/GameplayAbility.h"
#include "dazAda/Game/Characters/AcPlayerCharacter.h"
#include "dazAda/Game/Components/AC_CombatComponent.h"
#include "dazAda/Game/Components/AC_StatusEffectComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"


UACGA_FemaleKick::UACGA_FemaleKick()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    FGameplayTag Ability1Tag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Ability3"));
    AbilityTags.AddTag(Ability1Tag);
    ActivationOwnedTags.AddTag(Ability1Tag);
    ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill")));

    BasePhysicalDamage = 30.f;
    AD_Mult = 1.2f;
    BaseForce = 30.f;
    ForceMult = 1.2f;
}

void UACGA_FemaleKick::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    GEngine->AddOnScreenDebugMessage(
-1,           // Key: -1 for a unique message, or a specific key to update an existing message
5.0f,         // TimeToDisplay: How long the message stays on screen (in seconds)
FColor::Yellow, // Display color of the message
TEXT("entered!"));
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
        return;
    }

    // Cache character
    CachedCharacter = Cast<AAcBaseCharacter>(ActorInfo->AvatarActor.Get());
    if (!CachedCharacter)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
        return;
    }



    // Increment combo counter
    CurrentComboStep++;
    if (CurrentComboStep > MaxComboCount)
    {
        CurrentComboStep = 1; // Loop back to first combo
    }

    // Reset combo timer
    if (UWorld* World = CachedCharacter->GetWorld())
    {
        World->GetTimerManager().ClearTimer(ComboResetTimer);
        World->GetTimerManager().SetTimer(
            ComboResetTimer,
            this,
            &UACGA_FemaleKick::ResetCombo,
            ComboResetTime,
            false
        );
    }


    
    CachedCharacter->GetCombatCoreComponent()->OnFootTraceFinished.AddDynamic(
            this, &UACGA_FemaleKick::OnWeaponTraceFinished);
    

    // Get AnimInstance
    UAnimInstance* AnimInstance = CachedCharacter->GetMesh()->GetAnimInstance();
    if (!AnimInstance)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
        return;
    }
    GEngine->AddOnScreenDebugMessage(
-1,           // Key: -1 for a unique message, or a specific key to update an existing message
5.0f,         // TimeToDisplay: How long the message stays on screen (in seconds)
FColor::Yellow, // Display color of the message
TEXT("ended!"));
    float MontageDuration = PlayAbilityMontage();

}

// Called when montage starts blending out
void UACGA_FemaleKick::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{

}

// Called when montage fully ends
void UACGA_FemaleKick::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage != AbilityMontage) return;


    CachedCharacter->GetCombatCoreComponent()->OnFootTraceFinished.RemoveDynamic(
            this, &UACGA_FemaleKick::OnWeaponTraceFinished); 

    /*if (AAcPlayerCharacter* playerCharacter = Cast<AAcPlayerCharacter>(CachedCharacter))
    {
        playerCharacter->LockCameraToHead(false);
    }*/
    
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

float UACGA_FemaleKick::PlayAbilityMontage()
{
    if (!CachedCharacter || !AbilityMontage) return 0.f;

    UAnimInstance* AnimInstance = CachedCharacter->GetMesh()->GetAnimInstance();
    if (!AnimInstance) return 0.f;


    /*if (AAcPlayerCharacter* playerCharacter = Cast<AAcPlayerCharacter>(CachedCharacter))
    {
        playerCharacter->LockCameraToHead(true);
    }*/
    
    // Use ASC helper instead of raw Montage_Play
    float Duration = AnimInstance->Montage_Play(AbilityMontage, 1.0);

    if (Duration > 0.f)
    {
        FName SectionName = *FString::FromInt(CurrentComboStep);

        if (AbilityMontage->IsValidSectionName(SectionName))
        {
            AnimInstance->Montage_JumpToSection(SectionName, AbilityMontage);
        }
        
        // Bind delegates
        FOnMontageBlendingOutStarted BlendOutDelegate;
        BlendOutDelegate.BindUObject(this, &UACGA_FemaleKick::OnMontageBlendingOut);
        AnimInstance->Montage_SetBlendingOutDelegate(BlendOutDelegate, AbilityMontage);

        FOnMontageEnded MontageEndedDelegate;
        MontageEndedDelegate.BindUObject(this, &UACGA_FemaleKick::OnMontageEnded);
        AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, AbilityMontage);
    }

    return Duration;
}

void UACGA_FemaleKick::ResetCombo()
{
    CurrentComboStep = 0;
}

// todo. make LIGHT MID KB KD effect that each has stun duration and a cue to play animation montage
void UACGA_FemaleKick::OnWeaponTraceFinished(const TArray<FHitResult>& HitResults)
{

    AlreadyHitActors.Empty();
    if (DamageGameplayEffect)
    {
        FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffect, GetAbilityLevel());
        if (DamageEffectSpecHandle.IsValid())
        {
            // Pass the damage to the Damage Execution Calculation through a SetByCaller value on the GameplayEffectSpec
            
            
            for (const FHitResult& Hit : HitResults)
            {
                AActor* HitActor = Hit.GetActor();
                if (!HitActor || AlreadyHitActors.Contains(HitActor))
                {
                    continue; // already hit before
                }
                AlreadyHitActors.Add(HitActor);

                // Cast to your character class
                AAcBaseCharacter* TargetCharacter = Cast<AAcBaseCharacter>(Hit.GetActor());
                if (!TargetCharacter)
                {
                    continue;
                }
                // Get the ASC directly from your character
                UAbilitySystemComponent* TargetASC = TargetCharacter->GetAbilitySystemComponent();
                if (!TargetASC)
                {
                    continue;
                }

                float total_damage = BasePhysicalDamage + AD_Mult * CachedCharacter->GetBaseAD();
                DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), total_damage);

                float total_StanceDamage = BaseForce + ForceMult* CachedCharacter->GetBaseForce();
                DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.StanceDamage")), total_StanceDamage);
                
                FString HitName = Hit.GetActor()->GetName();
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Hit: %s"), *HitName));

                // Apply the damage GE
                DamageEffectSpecHandle.Data->GetContext().AddHitResult(Hit);
                TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
                TargetCharacter->GetStatusEffectComponent()->ApplyHitEffect(Hit,total_StanceDamage,total_damage);
                
            }
        }
        
    }


}