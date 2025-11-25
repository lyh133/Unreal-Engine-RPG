// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "dazAda.generated.h" 

#define ACTOR_ROLE_FSTRING *(FindObject<UEnum>(nullptr, TEXT("/Script/Engine.ENetRole"), true)->GetNameStringByValue(GetLocalRole()))
#define GET_ACTOR_ROLE_FSTRING(Actor) *(FindObject<UEnum>(nullptr, TEXT("/Script/Engine.ENetRole"), true)->GetNameStringByValue(Actor->GetLocalRole()))
UENUM(BlueprintType)
enum class EACHitReactDirection : uint8
{
	// 0
	None			UMETA(DisplayName = "None"),
	// 1
	Left 			UMETA(DisplayName = "Left"),
	// 2
	Front 			UMETA(DisplayName = "Front"),
	// 3
	Right			UMETA(DisplayName = "Right"),
	// 4
	Back			UMETA(DisplayName = "Back")
};

UENUM(BlueprintType)
enum class EACAbilityInputID : uint8
{
	// 0 None
	None			UMETA(DisplayName = "None"),
	// 1 Confirm
	Confirm			UMETA(DisplayName = "Confirm"),
	// 2 Cancel
	Cancel			UMETA(DisplayName = "Cancel"),
	// 3 LMB
	Ability1		UMETA(DisplayName = "Ability1"),
	// 4 RMB
	Ability2		UMETA(DisplayName = "Ability2"),
	// 5 Q
	Ability3		UMETA(DisplayName = "Ability3"),
	// 6 E
	Ability4		UMETA(DisplayName = "Ability4"),
	// 7 R
	Ability5		UMETA(DisplayName = "Ability5")
};

UENUM(BlueprintType)
enum class EOverlayLayers : uint8
{

	Base			UMETA(DisplayName = "Base"),
	Unarmed_Melee			UMETA(DisplayName = "Unarmed_Melee"),

};

UENUM(BlueprintType)
enum class EACMovementState : uint8
{
	None        UMETA(DisplayName = "None"),
	Grounded    UMETA(DisplayName = "Grounded"),
	InAir       UMETA(DisplayName = "In Air"),
	Mantling    UMETA(DisplayName = "Mantling"),
	Ragdoll     UMETA(DisplayName = "Ragdoll")
};

UENUM(BlueprintType)
enum class EACLocomotionState : uint8
{
	None		UMETA(DisplayName = "None"),
	Idle		UMETA(DisplayName = "Idle"),
	Walk		UMETA(DisplayName = "Walk"),
	Jog			UMETA(DisplayName = "Jog"),
	Sprint		UMETA(DisplayName = "Sprint")
};

UENUM(BlueprintType)
enum class EACBreathState : uint8
{
	None,
	Normal,
	Rest,
	Jog_Normal,
	Jog_Tired,
	Sprint_Normal,
	Sprint_Tired
};


USTRUCT(BlueprintType)
struct FCharacterGameplayStates
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EACMovementState MovementState = EACMovementState::None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EACLocomotionState LocomotionState = EACLocomotionState::Idle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EOverlayLayers OverlayState = EOverlayLayers::Base;
};

UENUM(BlueprintType, meta = (DisplayName = "Stimulus Sense"))
enum class EStimulusSenseType : uint8
{
	Unknown UMETA(DisplayName = "Unknown"),
	Sight   UMETA(DisplayName = "Sight"),
	Hearing UMETA(DisplayName = "Hearing"),
	Damage  UMETA(DisplayName = "Damage")
};

UENUM(BlueprintType, meta = (DisplayName = "State Tree Event"))
enum class EStateTreeEvent : uint8
{
	Unknown    UMETA(DisplayName = "Unknown"),
	Idle       UMETA(DisplayName = "Idle"),
	Patrol     UMETA(DisplayName = "Patrol"),
	Attack     UMETA(DisplayName = "Attack")
};

USTRUCT(BlueprintType)
struct FACDirectionalMontages
{
	GENERATED_BODY()

public:
	// Forward
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional Montages")
	UAnimMontage* Forward;

	// Forward Right
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional Montages")
	UAnimMontage* ForwardRight;

	// Forward Left
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional Montages")
	UAnimMontage* ForwardLeft;

	// Right
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional Montages")
	UAnimMontage* Right;

	// Left
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional Montages")
	UAnimMontage* Left;

	// Back
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional Montages")
	UAnimMontage* Back;

	// Back Right
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional Montages")
	UAnimMontage* BackRight;

	// Back Left
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional Montages")
	UAnimMontage* BackLeft;

public:
	// Helper: Get montage based on direction enum
	UAnimMontage* GetMontageFromDirection(EACHitReactDirection Direction) const
	{
		switch (Direction)
		{
		case EACHitReactDirection::Front: return Forward;
		case EACHitReactDirection::Left: return Left;
		case EACHitReactDirection::Right: return Right;
		case EACHitReactDirection::Back: return Back;
		default: return Forward;
		}
	}

	// Optional: 8-way lookup
	UAnimMontage* Get8WayMontage(const FVector& DirectionVector, const FVector& ForwardVector) const
	{
		const float Angle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(DirectionVector.GetSafeNormal2D(), ForwardVector.GetSafeNormal2D())));
		const FVector Cross = FVector::CrossProduct(ForwardVector, DirectionVector);

		const bool bRight = Cross.Z < 0;

		if (Angle <= 22.5f)
			return Forward;
		else if (Angle <= 67.5f)
			return bRight ? ForwardRight : ForwardLeft;
		else if (Angle <= 112.5f)
			return bRight ? Right : Left;
		else if (Angle <= 157.5f)
			return bRight ? BackRight : BackLeft;
		else
			return Back;
	}
};