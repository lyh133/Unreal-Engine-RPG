// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Touch.h"
#include "Perception/AISenseConfig_Prediction.h"
#include "GenericTeamAgentInterface.h"
#include "dazAda/dazAda.h"
#include "dazAda/Game/Characters/AcNPC.h"
#include "dazAda/Game/Components/AC_StateTreeAIComponent.h"
#include "AC_AIController.generated.h"

// Delegate for perception updates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FACStimulusEventSignature, AActor*, Actor, const FAIStimulus&, Stimulus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FACStimulusForgottenSignature, AActor*, Actor);
UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent, DisplayName = "NPC AI Controller"))
class DAZADA_API AAC_AIController : public AAIController
{
	GENERATED_BODY()

public:
	AAC_AIController();
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	UFUNCTION(BlueprintCallable, Category = "AI", meta = (DisplayName = "Get State Tree AI Component"))
	UAC_StateTreeAIComponent* GetStateTreeAIComponent() const { return StateTreeAIComponent; };
	
	UFUNCTION(BlueprintCallable, Category = "AI", meta = (DisplayName = "Get Current Stimulus Sense Type"))
	EStimulusSenseType GetCurrentStimulusSenseType() const { return CurrentStimulusSenseType; };



	/**
	 * Forget a specific actor from perception.
	 * @param ActorToForget The actor to forget.
	*/
	UFUNCTION(BlueprintCallable, Category = "AI")
	void ForgetPerceptionActor(AActor* ActorToForget);

	/**
	 * Forget multiple actors from perception.
	 * @param ActorsToForget The array of actors to forget.
	*/
	UFUNCTION(BlueprintCallable, Category = "AI")
	void ForgetPerceptionActors(const TArray<AActor*>& ActorsToForget);

	/**
	 * Returns all actors currently sensed by the Damage sense.
	 * This function retrieves all actors that have been sensed by the Damage sense.
	 * @return An array of actors currently sensed by the Damage sense.
	*/
	UFUNCTION(BlueprintCallable, Category = "AI")
	TArray<AActor*> GetAllDamageSensedActors() const;

	/**
	 * Returns all actors currently sensed by the Hearing sense.
	 * This function retrieves all actors that have been sensed by the Hearing sense.
	 * @return An array of actors currently sensed by the Hearing sense.
	*/
	UFUNCTION(BlueprintCallable, Category = "AI")
	TArray<AActor*> GetAllHeardActors() const;


	
	/**
	 * Returns all actors currently sensed by the Sight sense.
	 * This function retrieves all actors that have been sensed by the Sight sense.
	 * @return An array of actors currently sensed by the Sight sense.
	*/
	UFUNCTION(BlueprintCallable, Category = "Player|AI")
	TArray<AActor*> GetAllSeenActors() const;
	
	/**
	 * Report a damage event to the AI perception system.
	 * This function is used to report damage events to the AI perception system.
	 * @param DamagedActor The actor that was damaged.
	 * @param InstigatorActor The actor that caused the damage.
	 * @param DamageAmount The amount of damage inflicted.
	*/
	UFUNCTION(BlueprintCallable, Category = "AI|Perception")
	void ReportDamageEvent(AActor* DamagedActor, AActor* InstigatorActor, float DamageAmount);

	/**
	 * Report a noise event to the AI perception system.
	 * This function is used to report noise events to the AI perception system.
	 * @param NoiseInstigator The actor that made the noise.
	 * @param NoiseLocation The location where the noise was made.
	 * @param Loudness The loudness of the noise (default is 1.0).
	 * @param MaxRange The maximum range of the noise (default is 1200.0).
	*/
	UFUNCTION(BlueprintCallable, Category = "AI|Perception")
	void ReportNoiseEvent(AActor* NoiseInstigator, FVector NoiseLocation, float Loudness = 1.f, float MaxRange = 1200.f);




	

	UPROPERTY(BlueprintAssignable, Category = "AI|Events", meta = (DisplayName = "On Damage Stimulus Detected"))
	FACStimulusEventSignature OnDamageStimulusDetected;

	/** Broadcast delegates */
	UPROPERTY(BlueprintAssignable, Category = "AI|Events", meta = (DisplayName = "On Sight Stimulus Detected"))
	FACStimulusEventSignature OnSightStimulusDetected;

	UPROPERTY(BlueprintAssignable, Category = "AI|Events", meta = (DisplayName = "On Sight Stimulus Forgotten"))
	FACStimulusForgottenSignature OnSightStimulusForgotten;

	UPROPERTY(BlueprintAssignable, Category = "AI|Events", meta = (DisplayName = "On Hearing Stimulus Detected"))
	FACStimulusEventSignature OnHearingStimulusDetected;

	UPROPERTY(BlueprintAssignable, Category = "AI|Events", meta = (DisplayName = "On Hearing Stimulus Forgotten"))
	FACStimulusForgottenSignature OnHearingStimulusForgotten;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Output, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> AcquiredTarget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Output, meta = (AllowPrivateAccess = "true"))
	EStateTreeEvent CurrentStateTreeState;
	
protected:

	/*
	 * Functions
	*/

	// Perception updated callback
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void OnTargetPerceptionForgotten(AActor* Actor);
	
private:
	
	/** Enemy character this controller is managing */
	TObjectPtr<AAcNPC> OwningCharacter = nullptr;

	// Sight configuration object
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	// Hearing config
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

	// Damage config
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Damage> DamageConfig;

	// Prediction config
	UPROPERTY()
	TObjectPtr<UAISenseConfig_Prediction> PredictionConfig;
	
	UPROPERTY()
	EStimulusSenseType CurrentStimulusSenseType = EStimulusSenseType::Unknown;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAC_StateTreeAIComponent> StateTreeAIComponent;
	
	// Override team attitude to filter perception targets
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
};
