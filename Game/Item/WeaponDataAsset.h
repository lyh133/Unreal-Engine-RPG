// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "WeaponDataAsset.generated.h"

class UGameplayEffect;
class UGameplayAbility;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FWeaponStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Stats")
	float BaseAD;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Stats")
	float BaseAP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Stats")
	float BaseAS;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Stats")
	float AD_Multiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Stats")
	float AP_Multiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Stats")
	float AS_Multiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Stats")
	float Range;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon Stats")
	float BaseForce;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	float StaminaCost;

	

	// Add other stats as needed
};

UCLASS()
class DAZADA_API UWeaponDataAsset : public UItemDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* AttackSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* HitSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> OnEquipBuff;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWeaponStats WeaponStats;

	
};
