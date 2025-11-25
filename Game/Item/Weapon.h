// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

struct FGameplayAbilitySpecHandle;
class UGameplayEffect;
class UBoxComponent;
class UAcGameplayAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponTraceFinished, const TArray<FHitResult>&, HitResults);
UCLASS()
class DAZADA_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeapon();
	
	UFUNCTION(BlueprintCallable, Category =  "Weapon Function")
	void StartMeleeTrace();
	
	UFUNCTION(BlueprintCallable, Category =  "Weapon Function")
	void StopMeleeTrace();

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
	FOnWeaponTraceFinished OnWeaponTraceFinished;  
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Property")
	UBoxComponent* weaponBox;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon Property")
	USceneComponent* weaponTop;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Property")
	USceneComponent* weaponBot;

	FTimerHandle TimerHandle;
	
	UFUNCTION()
	void SphereTraceEvent();
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Ability")
	TArray<TSubclassOf<UAcGameplayAbility>> DefaultAbilities;

	// Keep handles so we can remove them later
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;
};


