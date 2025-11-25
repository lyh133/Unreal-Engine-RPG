#include "dazAda/Game/Components/AC_StatusEffectComponent.h"
#include "dazAda/Game/Characters/AcBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "AC_RagdollComponent.h"
#include "GameplayEffect.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

UAC_StatusEffectComponent::UAC_StatusEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Preload direction tags (same as AAcBaseCharacter)
	HitDirectionFrontTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Front"));
	HitDirectionBackTag  = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Back"));
	HitDirectionRightTag = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Right"));
	HitDirectionLeftTag  = FGameplayTag::RequestGameplayTag(FName("Effect.HitReact.Left"));
	DeadTag              = FGameplayTag::RequestGameplayTag(FName("State.Dead"));
	TiredTag = FGameplayTag::RequestGameplayTag(FName("State.Debuff.Tired"));
	JoggingTag = FGameplayTag::RequestGameplayTag(FName("Locomotion.MovementMode.Jog"));
	SprintingTag = FGameplayTag::RequestGameplayTag(FName("Locomotion.MovementMode.Sprint"));
}

void UAC_StatusEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedCharacter = Cast<AAcBaseCharacter>(GetOwner());
	if (CachedCharacter)
	{
		CachedASC = CachedCharacter->GetAbilitySystemComponent();
	}

	// Create audio components from sounds
	auto MakeAudio = [&](USoundBase* Sound) -> UAudioComponent*
	{
		if (!Sound || !CachedCharacter) return nullptr;

		UAudioComponent* Comp = NewObject<UAudioComponent>(CachedCharacter);
		Comp->bAutoActivate = false;
		Comp->SetSound(Sound);
		Comp->AttachToComponent(
			CachedCharacter->GetRootComponent(),
			FAttachmentTransformRules::KeepRelativeTransform
		);
		Comp->RegisterComponent();
		return Comp;
	};

	NormalBreathAudio       = MakeAudio(NormalBreathSound);
	RestBreathAudio         = MakeAudio(RestBreathSound);
	JogBreathAudio          = MakeAudio(JogBreathSound);
	JogTiredBreathAudio     = MakeAudio(JogTiredBreathSound);
	SprintBreathAudio       = MakeAudio(SprintBreathSound);
	SprintTiredBreathAudio  = MakeAudio(SprintTiredBreathSound);

	GetWorld()->GetTimerManager().SetTimer(
	SlowTickTimerHandle,
	this,
	&UAC_StatusEffectComponent::SlowTick,
	0.5f,      // interval
	true       // looping
);
}


void UAC_StatusEffectComponent::SlowTick()
{
	if (!CachedCharacter || !CachedASC)
		return;
	
	SetBreathState(GetBreathState());
}


EACHitReactDirection UAC_StatusEffectComponent::GetHitReactDirection(const FVector& ImpactPoint)
{
	if (!CachedCharacter) return EACHitReactDirection::Front;

	const FVector& ActorLocation = CachedCharacter->GetActorLocation();
	float DistFrontBack = FVector::PointPlaneDist(ImpactPoint, ActorLocation, CachedCharacter->GetActorRightVector());
	float DistRightLeft = FVector::PointPlaneDist(ImpactPoint, ActorLocation, CachedCharacter->GetActorForwardVector());

	if (FMath::Abs(DistFrontBack) <= FMath::Abs(DistRightLeft))
	{
		return (DistRightLeft >= 0) ? EACHitReactDirection::Front : EACHitReactDirection::Back;
	}
	else
	{
		return (DistFrontBack >= 0) ? EACHitReactDirection::Right : EACHitReactDirection::Left;
	}
}

void UAC_StatusEffectComponent::PlayHitReact_Implementation(EACHitReactDirection HitDirection, AActor* DamageCauser, float StanceDmg)
{
	if (CachedCharacter && CachedCharacter->bIsAlive)
	{
		// Currently empty in AAcBaseCharacter — keeping same behavior
		/*if (HitDirection == HitDirectionLeftTag)
{
	ShowHitReact.Broadcast(EACHitReactDirection::Left);
}
else if (HitDirection == HitDirectionFrontTag)
{
	ShowHitReact.Broadcast(EACHitReactDirection::Front);
}
else if (HitDirection == HitDirectionRightTag)
{
	ShowHitReact.Broadcast(EACHitReactDirection::Right);
}
else if (HitDirection == HitDirectionBackTag)
{
	ShowHitReact.Broadcast(EACHitReactDirection::Back);
}*/
	}
}

bool UAC_StatusEffectComponent::PlayHitReact_Validate(EACHitReactDirection HitDirection, AActor* DamageCauser, float StanceDmg)
{
	return true;
}

void UAC_StatusEffectComponent::ApplyHitEffect(const FHitResult& HitResult, float StanceDamage, float Damage)
{
	if (!CachedASC) return;

	TSubclassOf<UGameplayEffect> HitEffect = nullptr;
	TSubclassOf<UGameplayEffect> StatusEffect = nullptr;

	float DisableDuration = 1.0f;
	float StunDuration = 2.5f;
	float PercentStance = CachedCharacter ? CachedCharacter->GetStancePercentage() : 1.0f;
	float PercentHealth = CachedCharacter ? CachedCharacter->GetHealthPercentage() : 1.0f;
	if (PercentStance >= 0.8 && PercentHealth >= 0.8)
	{
		HitEffect = GE_LightReact;
		StatusEffect = GE_Disabled;
	}
	else if (PercentStance >= 0.5 && PercentHealth >= 0.5)
	{
		HitEffect = GE_MidReact;
		StatusEffect = GE_Stunned;
	}
	else if (PercentStance >= 0.25 && PercentHealth >= 0.25)
	{
		HitEffect = GE_KBReact;
		StatusEffect = GE_Stunned;
	}
	else
	{
		HitEffect = GE_KDReact;
		StatusEffect = GE_Stunned;
		if (UAC_RagdollComponent* RagdollComponent = CachedCharacter->GetRagdollComponent())
		{
			RagdollComponent->StartRagdollDelayed(0.5f);
		}
	}

	if (HitEffect)
	{
		FGameplayEffectSpecHandle SpecHandle = CachedASC->MakeOutgoingSpec(HitEffect, 1.f, CachedASC->MakeEffectContext());
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->GetContext().AddHitResult(HitResult);
			CachedASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	if (StatusEffect)
	{
		FGameplayEffectSpecHandle SpecHandle = CachedASC->MakeOutgoingSpec(StatusEffect, 1.f, CachedASC->MakeEffectContext());
		if (SpecHandle.IsValid())
		{
			if (StatusEffect == GE_Disabled)
				SpecHandle.Data->SetDuration(DisableDuration, true);
			else if (StatusEffect == GE_Stunned)
				SpecHandle.Data->SetDuration(StunDuration, true);

			CachedASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void UAC_StatusEffectComponent::HandleTiredStateChanged(bool bTired)
{
	if (!CachedASC)
		return;

	if (bTired)
	{
		// Apply only if not already applied
		if (!TiredEffectHandle.IsValid() && TiredGameplayEffect)
		{
			FGameplayEffectContextHandle Ctx = CachedASC->MakeEffectContext();
			FGameplayEffectSpecHandle Spec = 
				CachedASC->MakeOutgoingSpec(TiredGameplayEffect, 1.f, Ctx);

			if (Spec.IsValid())
			{
				TiredEffectHandle =
					CachedASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			}
		}
	}
	else
	{
		if (TiredEffectHandle.IsValid())
		{
			CachedASC->RemoveActiveGameplayEffect(TiredEffectHandle);
			TiredEffectHandle.Invalidate();
		}
	}
}


void UAC_StatusEffectComponent::SetBreathState(EACBreathState State)
{
	// Pick the NEW desired audio component
	UAudioComponent* Desired = nullptr;

	switch (State)
	{
	case EACBreathState::Normal:
		Desired = NormalBreathAudio;
		break;

	case EACBreathState::Rest:
		Desired = RestBreathAudio;
		break;

	case EACBreathState::Jog_Normal:
		Desired = JogBreathAudio;
		break;

	case EACBreathState::Jog_Tired:
		Desired = JogTiredBreathAudio;
		break;

	case EACBreathState::Sprint_Normal:
		Desired = SprintBreathAudio;
		break;

	case EACBreathState::Sprint_Tired:
		Desired = SprintTiredBreathAudio;
		break;
	}

	// If nothing should play, stop everything
	if (!Desired)
	{
		TArray<UAudioComponent*> AllBreaths = {
			NormalBreathAudio, RestBreathAudio,
			JogBreathAudio, JogTiredBreathAudio,
			SprintBreathAudio, SprintTiredBreathAudio
		};

		for (UAudioComponent* C : AllBreaths)
		{
			if (C && C->IsPlaying())
			{
				C->FadeOut(0.2f, 0.f);
			}
		}
		return;
	}

	// Stop all other sounds EXCEPT the desired one
	TArray<UAudioComponent*> AllBreaths = {
		NormalBreathAudio, RestBreathAudio,
		JogBreathAudio, JogTiredBreathAudio,
		SprintBreathAudio, SprintTiredBreathAudio
	};

	for (UAudioComponent* C : AllBreaths)
	{
		if (C && C != Desired && C->IsPlaying())
		{
			C->FadeOut(0.2f, 0.f);
		}
	}

	// If desired is already playing → DO NOTHING
	if (Desired->IsPlaying())
		return;

	// Play the new one
	Desired->FadeIn(0.2f);
}

EACBreathState UAC_StatusEffectComponent::GetBreathState() const
{
	if (!CachedASC)
		return EACBreathState::None;

	const bool bTired     = CachedASC->HasMatchingGameplayTag(TiredTag);
	const bool bJogging   = CachedASC->HasMatchingGameplayTag(JoggingTag);
	const bool bSprinting = CachedASC->HasMatchingGameplayTag(SprintingTag);

	// Sprinting
	if (bSprinting)
		return bTired ? EACBreathState::Sprint_Tired : EACBreathState::Sprint_Normal;

	// Jogging
	if (bJogging)
		return bTired ? EACBreathState::Jog_Tired : EACBreathState::Jog_Normal;

	// Idle / walking
	if (bTired)
		return EACBreathState::Rest;      // tired but not moving fast

	return EACBreathState::Normal;        // default normal breathing
}