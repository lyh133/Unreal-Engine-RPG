// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "dazAda/Game/Characters/AcBaseCharacter.h"
#include "StateTreeReference.h"
#include "GenericTeamAgentInterface.h"
#include "AcNPC.generated.h"

class UStateTree;

UCLASS()
class DAZADA_API AAcNPC : public AAcBaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAcNPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnDeath() override;
	FGenericTeamId TeamId = FGenericTeamId(2); // 1 = Player, 2 = Enemy
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void UnPossessed() override;

	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

	// Returns the State Tree asset for this character.
	// This is used for AI behavior logic.
	UFUNCTION(BlueprintPure, Category = "Player|AI", meta = (DisplayName = "Get State Tree", ReturnDisplayName = "State Tree"))
	UStateTree* GetStateTree() { return StateTree; }
private:
	// Reference to the StateTree asset for AI logic
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|AI", meta = (AllowPrivateAccess = "true"))
	UStateTree* StateTree;


};
