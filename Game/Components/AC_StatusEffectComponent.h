#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "dazAda/dazAda.h"
#include "AC_StatusEffectComponent.generated.h"

class AAcBaseCharacter;
class UAbilitySystemComponent;
class UGameplayEffect;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterBaseHitReactDelegate, EACHitReactDirection, Direction);

UCLASS(BlueprintType, Blueprintable, ClassGroup = (ACGA),
	meta = (BlueprintSpawnableComponent, DisplayName = "ACGA Hit Reaction Component", Tooltip = "Handles hit reactions for characters"))
class DAZADA_API UAC_StatusEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_StatusEffectComponent();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effects")
	TSubclassOf<UGameplayEffect> TiredGameplayEffect;
	// Store the active handle so we can remove the effect later
	FActiveGameplayEffectHandle TiredEffectHandle;

	// Audio component for heavy breathing
	UPROPERTY()
	UAudioComponent* NormalBreathAudio = nullptr;

	UPROPERTY()
	UAudioComponent* RestBreathAudio = nullptr;

	UPROPERTY()
	UAudioComponent* JogBreathAudio = nullptr;

	UPROPERTY()
	UAudioComponent* JogTiredBreathAudio = nullptr;

	UPROPERTY()
	UAudioComponent* SprintBreathAudio = nullptr;

	UPROPERTY()
	UAudioComponent* SprintTiredBreathAudio = nullptr;
	
private:
	UPROPERTY(Transient)
	TObjectPtr<AAcBaseCharacter> CachedCharacter;

	UPROPERTY(Transient)
	TObjectPtr<UAbilitySystemComponent> CachedASC;
	
	FTimerHandle SlowTickTimerHandle;

	FGameplayTag TiredTag;
	FGameplayTag JoggingTag;
	FGameplayTag SprintingTag;

private:
	void SlowTick();
	
	EACBreathState GetBreathState() const;
	void SetBreathState(EACBreathState State);
public:
	// === Gameplay Effects ===
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arcane|HitReact")
	TSubclassOf<UGameplayEffect> GE_LightReact;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arcane|HitReact")
	TSubclassOf<UGameplayEffect> GE_MidReact;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arcane|HitReact")
	TSubclassOf<UGameplayEffect> GE_KBReact;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arcane|HitReact")
	TSubclassOf<UGameplayEffect> GE_KDReact;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arcane|HitReact")
	TSubclassOf<UGameplayEffect> GE_Disabled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arcane|HitReact")
	TSubclassOf<UGameplayEffect> GE_Stunned;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arcane|HitReact")
	TSubclassOf<UGameplayEffect> GE_Rooted;

	// === Hit Direction Gameplay Tags ===
	FGameplayTag HitDirectionFrontTag;
	FGameplayTag HitDirectionBackTag;
	FGameplayTag HitDirectionRightTag;
	FGameplayTag HitDirectionLeftTag;
	FGameplayTag DeadTag;

	// === Montages (copied from character) ===
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arcane|Montage")
	FACDirectionalMontages LightHitReactionMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arcane|Montage")
	FACDirectionalMontages MidHitReactionMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arcane|Montage")
	FACDirectionalMontages KBHitReactionMontages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Arcane|Montage")
	FACDirectionalMontages KDHitReactionMontages;

	UPROPERTY(BlueprintAssignable, Category = "Arcane|Character")
	FCharacterBaseHitReactDelegate ShowHitReact;
	
	// ==========================================
	//  Breath Sounds (Assignable in Blueprint)
	// ==========================================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effects|Audio")
	USoundBase* NormalBreathSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effects|Audio")
	USoundBase* RestBreathSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effects|Audio")
	USoundBase* JogBreathSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effects|Audio")
	USoundBase* JogTiredBreathSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effects|Audio")
	USoundBase* SprintBreathSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effects|Audio")
	USoundBase* SprintTiredBreathSound = nullptr;
public:
	// === Core Functionalities (identical to AAcBaseCharacter) ===

	UFUNCTION(BlueprintCallable, Category = "Arcane|HitReact")
	EACHitReactDirection GetHitReactDirection(const FVector& ImpactPoint);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void PlayHitReact(EACHitReactDirection HitDirection, AActor* DamageCauser, float StanceDmg);
	void PlayHitReact_Implementation(EACHitReactDirection HitDirection, AActor* DamageCauser, float StanceDmg);
	bool PlayHitReact_Validate(EACHitReactDirection HitDirection, AActor* DamageCauser, float StanceDmg);

	UFUNCTION(BlueprintCallable, Category = "Arcane|HitReact")
	void ApplyHitEffect(const FHitResult& HitResult, float StanceDamage, float Damage);

	void HandleTiredStateChanged(bool bTired);
};