// ACGA_FemalePunch.cpp
#include "ACGA_FemalePunch.h"
#include "AbilitySystemComponent.h"
#include "dazAda/Game/Interface/CharacterDataInterface.h"
#include "dazAda/Game/Characters/AcBaseCharacter.h"
#include "Animation/AnimInstance.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Actor.h"
#include "dazAda/Game/Components/AC_CombatComponent.h"
#include "dazAda/Game/Components/AC_StatusEffectComponent.h"
#include "Kismet/KismetMathLibrary.h"

UACGA_FemalePunch::UACGA_FemalePunch()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    FGameplayTag Ability1Tag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Ability1"));
    AbilityTags.AddTag(Ability1Tag);
    ActivationOwnedTags.AddTag(Ability1Tag);
    ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill")));

    BasePhysicalDamage = 0.f;
    AD_Mult = 1.0f;
    BaseForce = 0.f;
    ForceMult = 1.0f;
}

void UACGA_FemalePunch::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
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


    CachedCharacter->GetCombatCoreComponent()->OnFistTraceFinished.AddDynamic(
            this, &UACGA_FemalePunch::OnWeaponTraceFinished);
    

    // Get AnimInstance
    UAnimInstance* AnimInstance = CachedCharacter->GetMesh()->GetAnimInstance();
    if (!AnimInstance)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
        return;
    }
    

    // Interface call: upper body active
    if (AnimInstance->GetClass()->ImplementsInterface(UCharacterDataInterface::StaticClass()))
    {
        ICharacterDataInterface::Execute_ReceiveUpperBody(AnimInstance, true);
    }

    float MontageDuration = PlayAbilityMontage();

}

// Called when montage starts blending out
void UACGA_FemalePunch::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{

}

// Called when montage fully ends
void UACGA_FemalePunch::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage != AbilityMontage) return;
    if (!bInterrupted)
    {
        if (!CachedCharacter) return;
        UAnimInstance* AnimInstance = CachedCharacter->GetMesh()->GetAnimInstance();
        if (AnimInstance && AnimInstance->GetClass()->ImplementsInterface(UCharacterDataInterface::StaticClass()))
        {
            ICharacterDataInterface::Execute_ReceiveUpperBody(AnimInstance, false);
        }
    }

    CachedCharacter->GetCombatCoreComponent()->OnFistTraceFinished.RemoveDynamic(
            this, &UACGA_FemalePunch::OnWeaponTraceFinished); 

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

float UACGA_FemalePunch::PlayAbilityMontage()
{
    if (!CachedCharacter || !AbilityMontage) return 0.f;

    UAnimInstance* AnimInstance = CachedCharacter->GetMesh()->GetAnimInstance();
    if (!AnimInstance) return 0.f;

    // Use ASC helper instead of raw Montage_Play
    float Duration = AnimInstance->Montage_Play(AbilityMontage, 1.0);

    if (Duration > 0.f)
    {
        // Bind delegates
        FOnMontageBlendingOutStarted BlendOutDelegate;
        BlendOutDelegate.BindUObject(this, &UACGA_FemalePunch::OnMontageBlendingOut);
        AnimInstance->Montage_SetBlendingOutDelegate(BlendOutDelegate, AbilityMontage);

        FOnMontageEnded MontageEndedDelegate;
        MontageEndedDelegate.BindUObject(this, &UACGA_FemalePunch::OnMontageEnded);
        AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, AbilityMontage);
    }

    return Duration;
}

// todo. make LIGHT MID KB KD effect that each has stun duration and a cue to play animation montage
void UACGA_FemalePunch::OnWeaponTraceFinished(const TArray<FHitResult>& HitResults)
{

    AlreadyHitActors.Empty();
for (const FHitResult& Hit : HitResults)
{
    AActor* HitActor = Hit.GetActor();
    if (!HitActor || AlreadyHitActors.Contains(HitActor))
        continue;

    AlreadyHitActors.Add(HitActor);

    // Cast once
    AAcBaseCharacter* TargetCharacter = Cast<AAcBaseCharacter>(HitActor);
    if (!TargetCharacter)
        continue;

    UAbilitySystemComponent* TargetASC = TargetCharacter->GetAbilitySystemComponent();
    UAbilitySystemComponent* SourceASC = CachedCharacter->GetAbilitySystemComponent();
    if (!TargetASC || !SourceASC)
        continue;

    //----------------------------------------------------
    // 1. USE ATTACKER STATS, never target stats
    //----------------------------------------------------
    const float total_damage =
        BasePhysicalDamage + AD_Mult * CachedCharacter->GetBaseAD();

    const float total_StanceDamage =
        BaseForce + ForceMult * CachedCharacter->GetBaseForce();

    //----------------------------------------------------
    // 2. Create a NEW GE Context per target (important!)
    //----------------------------------------------------
    FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
    Context.AddHitResult(Hit);
    Context.AddSourceObject(CachedCharacter);

    //----------------------------------------------------
    // 3. Create a NEW GE Spec per target (never reuse!)
    //----------------------------------------------------
    FGameplayEffectSpecHandle SpecHandle =
        SourceASC->MakeOutgoingSpec(DamageGameplayEffect, 1.f, Context);

    if (!SpecHandle.IsValid())
        continue;

    //----------------------------------------------------
    // 4. Apply SetByCaller magnitudes to the NEW Spec
    //----------------------------------------------------
    SpecHandle.Data->SetSetByCallerMagnitude(
        FGameplayTag::RequestGameplayTag("Data.Damage"),
        total_damage
    );

    SpecHandle.Data->SetSetByCallerMagnitude(
        FGameplayTag::RequestGameplayTag("Data.StanceDamage"),
        total_StanceDamage
    );

    //----------------------------------------------------
    // 5. Apply GE to target
    //----------------------------------------------------
    TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

    //----------------------------------------------------
    // 6. Gameplay-side reaction (hit VFX, sounds, etc.)
    //----------------------------------------------------
    TargetCharacter->GetStatusEffectComponent()->ApplyHitEffect(
        Hit,
        total_StanceDamage,
        total_damage
    );
}


}