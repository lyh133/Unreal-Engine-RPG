#pragma once

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "dazAda/Game/AbilitySystem/AcAbilitySystemComponent.h"
#include "dazAda/dazAda.h"
#include "dazAda/Game/Interface/CharacterDataInterface.h"
#include "dazAda/Game/Item/Weapon.h"
#include "GenericTeamAgentInterface.h"
#include "AcBaseCharacter.generated.h"

class UAC_StatusEffectComponent;
class UAC_RagdollComponent;
class USphereComponent;
class UAC_CombatComponent;
class UAcCharacterStatusComponent;
//implement movementsystem  partial
//implement camera partial
//implement health
//implement ACAbilitySystemComponent
//implement player controller    -------------partially done
//implement abstract class  public IGameplayTagAssetInterface
//setup player input.  done
class UAcCombatAttributeSet;
class UGameplayEffect;
class USpringArmComponent;
class UCameraComponent;
class AActor;
class AController;


UCLASS()
class DAZADA_API AAcBaseCharacter : public ACharacter, public IAbilitySystemInterface, public IGameplayCueInterface, public ICharacterDataInterface , public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAcBaseCharacter(const  FObjectInitializer& ObjectInitializer= FObjectInitializer::Get());

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Arcane|Character")
	USkeletalMeshComponent* UpperBody;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Arcane|Character")
	USkeletalMeshComponent* LowerBody;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Arcane|Character")
	USkeletalMeshComponent* FootLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Arcane|Character")
	USkeletalMeshComponent* FootRight;
	
	UAcAbilitySystemComponent* GetAcAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void PreInitializeComponents() override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	
	UFUNCTION(BlueprintCallable, Category = "Arcane|Character")
	void SetIsAlive();
	UPROPERTY(BlueprintReadOnly, Category = "Arcane|Character")
	bool bIsAlive = true;
	

	UFUNCTION(BlueprintCallable, Category = "Arcane|Character|Attributes")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category = "Arcane|Character|Attributes")
	float GetStance() const;
	
	UFUNCTION(BlueprintCallable, Category = "Arcane|Character|Attributes")
	float GetStancePercentage() const;

	UFUNCTION(BlueprintCallable, Category = "Arcane|Character|Attributes")
	float GetHealthPercentage() const;
	
	UFUNCTION(BlueprintCallable, Category = "Arcane|Character|Attributes")
	float GetBaseAD() const;
	UFUNCTION(BlueprintCallable, Category = "Arcane|Character|Attributes")
	float GetBaseForce() const;
	
	UAC_CombatComponent* GetCombatCoreComponent() const
	{
		return CombatCoreComponent;
	}

	UAC_StatusEffectComponent* GetStatusEffectComponent() const
	{
		return StatusEffectComponent;
	}

	UAC_RagdollComponent* GetRagdollComponent() const
	{
		return RagdollComponent;
	}
	
	UPROPERTY(BlueprintReadWrite, Category="Arcane")
	EACMovementState CurrentMovementState = EACMovementState::None;
	UPROPERTY(BlueprintReadWrite, Category="Arcane")
	EACLocomotionState CurrentLocomotionState = EACLocomotionState::None;
	UPROPERTY(BlueprintReadWrite, Category="Arcane")
	EOverlayLayers currentOverlayState = EOverlayLayers::Base;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arcane")
	bool lockCamera = false;
	
	/*UFUNCTION(BlueprintCallable, Category = "Arcane|Character|State")
	bool IsRagdoll() const { return bIsRagdoll; }
	
	UFUNCTION(BlueprintCallable, Category = "Arcane|Character|State")
	void SetIsRagdoll(bool bNewState)  { bIsRagdoll = bNewState;}*/

	UFUNCTION(BlueprintCallable, Category = "Arcane|Character|Weapon")
	void EquipWeapon(AWeapon* NewWeapon);
	
	UFUNCTION(BlueprintCallable, Category = "Arcane|Character|Weapon")
	void UnequipWeapon();
	
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }


protected:
	//virtual void Tick(float DeltaTime) override;
	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();

	void OnMovementAccelerationChanged(const FOnAttributeChangeData& Data) const;
	void OnMovementSpeedChanged(const FOnAttributeChangeData& Data) const;
	virtual void OnDeath();
	
	UPROPERTY(EditDefaultsOnly, Category = "Arcane|Ability")
	TObjectPtr<UAcCombatAttributeSet> CombatAttributeSet;

	UPROPERTY(EditDefaultsOnly, Category = "Arcane|Ability")
	TSubclassOf<UGameplayEffect> DefaultGameplayEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Arcane|Ability")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;
	
	UPROPERTY()
	TObjectPtr<UAcAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAcCharacterStatusComponent> CharacterStatusComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arcane Combat System", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAC_CombatComponent> CombatCoreComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Arcane Combat System", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAC_StatusEffectComponent> StatusEffectComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Arcane Combat System", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAC_RagdollComponent> RagdollComponent = nullptr;
	
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Arcane|Ability")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;
	

	
	UAcCombatAttributeSet* GetCombatAttributeSet() const;
	
	void GiveDefaultAbilities();
	void InitDefaultAbilities() const;
	void InitializeGameplayTags();
	virtual void InitStartupEffects();
	
	UFUNCTION(BlueprintCallable, Category = "Arcane")
	void UpdateMovementState();
	
	
	//unused, we have component already
	UFUNCTION(BlueprintCallable, Category = "Arcane")
	void UpdateRagdoll();
	UPROPERTY(BlueprintReadOnly, Category = "Arcane|Ragdoll System")
	bool bRagdollOnGround = false;
	UPROPERTY(BlueprintReadOnly, Category = "Arcane|Ragdoll System")
	bool bRagdollFaceUp = false;
	UPROPERTY(BlueprintReadOnly, Category = "Arcane|Ragdoll System")
	FVector LastRagdollVelocity;


	FGenericTeamId TeamId = FGenericTeamId(1);
};


inline void AAcBaseCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

inline void AAcBaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

