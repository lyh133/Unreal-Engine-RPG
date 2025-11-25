// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Item.generated.h"

class UItemDataAsset;

UCLASS()
class DAZADA_API AItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItem();
	
	UPROPERTY(EditAnywhere)
	UItemDataAsset* ItemData;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ItemMesh;
protected:
	// Called when the game starts or when spawned
	USceneComponent* rootComponent;

public:
	// Called every frame

};
