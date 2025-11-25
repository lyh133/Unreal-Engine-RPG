// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AC_RagdollComponent.generated.h"


class AAcBaseCharacter;

UCLASS(
	BlueprintType,
	Blueprintable,
	ClassGroup = (ACGA),
	meta = (
		BlueprintSpawnableComponent,
		DisplayName = "ACGA ragdoll Component",
		Description = "manage ragoll functionality",
		Tooltip = "Handles ragoll functionality"
	)
)
class DAZADA_API UAC_RagdollComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAC_RagdollComponent();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* GetUpFaceUpMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* GetUpFaceDownMontage;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	
	UPROPERTY(BlueprintReadOnly, Category = "Arcane|Ragdoll System")
	bool bRagdollOnGround = false;
	UPROPERTY(BlueprintReadOnly, Category = "Arcane|Ragdoll System")
	bool bRagdollFaceUp = false;
	UPROPERTY(BlueprintReadOnly, Category = "Arcane|Ragdoll System")
	FVector LastRagdollVelocity;
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable, Category = "Arcane")
	void UpdateRagdoll();
	UFUNCTION(BlueprintCallable, Category = "Arcane")
	void StartRagdoll();

	void StartRagdollDelayed(float Delay);

	UFUNCTION(BlueprintCallable, Category = "Arcane")
	void EndRagdoll();

	
private:
	UPROPERTY(Transient)
	TObjectPtr<AAcBaseCharacter> CachedCharacter;
	
	UPROPERTY(Transient)
	TObjectPtr<UAbilitySystemComponent> CachedASC;

	FTimerHandle RagdollGetUpTimer;
	FTimerHandle RagdollDelayHandle;

	void ClearStunTag();
	
};
