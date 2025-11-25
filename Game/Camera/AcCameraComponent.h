// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "AcCameraComponent.generated.h"

/**
 * 
 */
UCLASS()
class DAZADA_API UAcCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
	
public:
	UAcCameraComponent(const FObjectInitializer& ObjectInitializer);
	// Returns the camera component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Lyra|Camera")
	static UAcCameraComponent* FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UAcCameraComponent>() : nullptr); }
	
	// Returns the target actor that the camera is looking at.
	virtual AActor* GetTargetActor() const { return GetOwner(); }
};
