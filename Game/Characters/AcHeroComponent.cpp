// Fill out your copyright notice in the Description page of Project Settings.


#include "AcHeroComponent.h"



UAcHeroComponent::UAcHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


void UAcHeroComponent::OnRegister()
{
	Super::OnRegister();
}

void UAcHeroComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAcHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::BeginPlay();
}

void UAcHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	
}