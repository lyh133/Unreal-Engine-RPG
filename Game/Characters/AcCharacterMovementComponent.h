// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "AcCharacterMovementComponent.generated.h"


class UObject;

UCLASS(Config = Game)
class DAZADA_API UAcCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAcCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

protected:

	virtual void InitializeComponent() override;	
};
