#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "dazAda/Game/Item/Weapon.h"
#include "AC_CombatComponent.generated.h"

// ===========================
// Delegates
// ===========================
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFistTraceFinished, const TArray<FHitResult>&, HitResults);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFootTraceFinished, const TArray<FHitResult>&, HitResults);

class AAcBaseCharacter;
class USphereComponent;
class UAbilitySystemComponent;

UCLASS(
	BlueprintType,
	Blueprintable,
	ClassGroup = (ACGA),
	meta = (
		BlueprintSpawnableComponent,
		DisplayName = "ACGA Combat Core Component",
		Description = "Central coordinator for combat",
		Tooltip = "Handles combat logic and melee tracing"
	)
)
class DAZADA_API UAC_CombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// ===========================
	// Constructor / Lifecycle
	// ===========================
	UAC_CombatComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// ===========================
	// Weapon Handling
	// ===========================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arcane|Combat")
	TSubclassOf<AWeapon> EquippedWeaponClass;

	// ===========================
	// Hitbox Components
	// ===========================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arcane|Combat")
	TObjectPtr<USphereComponent> FistLeftHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arcane|Combat")
	TObjectPtr<USphereComponent> FistRightHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arcane|Combat")
	TObjectPtr<USphereComponent> FootLeftHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arcane|Combat")
	TObjectPtr<USphereComponent> FootRightHitbox;

	// ===========================
	// Accessors
	// ===========================
	UFUNCTION(BlueprintCallable, Category = "Arcane|Combat")
	USphereComponent* GetFistLeftHitbox() const;

	UFUNCTION(BlueprintCallable, Category = "Arcane|Combat")
	USphereComponent* GetFistRightHitbox() const;

	UFUNCTION(BlueprintCallable, Category = "Arcane|Combat")
	USphereComponent* GetFootLeftHitbox() const;

	UFUNCTION(BlueprintCallable, Category = "Arcane|Combat")
	USphereComponent* GetFootRightHitbox() const;

	// ===========================
	// Trace Events
	// ===========================
	UFUNCTION()
	void LeftFistTraceEvent();

	UFUNCTION()
	void RightFistTraceEvent();

	UFUNCTION()
	void LeftFootTraceEvent();

	UFUNCTION()
	void RightFootTraceEvent();

	// ===========================
	// Trace Controls (for AnimNotify or Gameplay Ability)
	// ===========================
	UFUNCTION(BlueprintCallable, Category = "Arcane|Melee")
	void StartRightFistTrace();

	UFUNCTION(BlueprintCallable, Category = "Arcane|Melee")
	void StopRightFistTrace();

	UFUNCTION(BlueprintCallable, Category = "Arcane|Melee")
	void StartLeftFistTrace();

	UFUNCTION(BlueprintCallable, Category = "Arcane|Melee")
	void StopLeftFistTrace();

	UFUNCTION(BlueprintCallable, Category = "Arcane|Ability")
	void StartRightFootTrace();

	UFUNCTION(BlueprintCallable, Category = "Arcane|Ability")
	void StopRightFootTrace();

	UFUNCTION(BlueprintCallable, Category = "Arcane|Ability")
	void StartLeftFootTrace();

	UFUNCTION(BlueprintCallable, Category = "Arcane|Ability")
	void StopLeftFootTrace();

	// ===========================
	// Delegates (Blueprint Assignable)
	// ===========================
	UPROPERTY(BlueprintAssignable, Category = "Arcane|Events")
	FOnFistTraceFinished OnFistTraceFinished;

	UPROPERTY(BlueprintAssignable, Category = "Arcane|Events")
	FOnFootTraceFinished OnFootTraceFinished;

private:
	// ===========================
	// Internal Helpers
	// ===========================
	void PerformTrace(USphereComponent* Hitbox, FOnFistTraceFinished& Delegate);
	void PerformTrace(USphereComponent* Hitbox, FOnFootTraceFinished& Delegate);

	void StartTraceTimer(void (UAC_CombatComponent::*TraceFunc)());
	void StopTraceTimer();

private:
	// ===========================
	// Cached References
	// ===========================
	UPROPERTY(Transient)
	TObjectPtr<AAcBaseCharacter> CachedCharacter;

	UPROPERTY(Transient)
	TObjectPtr<UAbilitySystemComponent> CachedASC;

	UPROPERTY(Transient)
	TObjectPtr<AWeapon> EquippedWeapon;

	// ===========================
	// Timer Handle
	// ===========================
	FTimerHandle TimerHandle;
};