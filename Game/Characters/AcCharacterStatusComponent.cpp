// Fill out your copyright notice in the Description page of Project Settings.


#include "AcCharacterStatusComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "dazAda/Game/AbilitySystem/Attributes/AcCombatAttributeSet.h"


// Sets default values for this component's properties
UAcCharacterStatusComponent::UAcCharacterStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}


// Called when the game starts
void UAcCharacterStatusComponent::BeginPlay()
{
	Super::BeginPlay();
	
	AActor* Owner = GetOwner();
	const IAbilitySystemInterface* AbilityOwner = Cast<IAbilitySystemInterface>(Owner);
	if (!AbilityOwner) return;

	AbilitySystemComponent = AbilityOwner->GetAbilitySystemComponent();
	if (AbilitySystemComponent)
	{
		CombatAttributeSet = AbilitySystemComponent->GetSet<UAcCombatAttributeSet>();
	}

	// Optional: log if failed
	if (!AbilitySystemComponent || !CombatAttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterStatusComponent: Failed to get ASC or AttributeSet"));
	}
	GetWorld()->GetTimerManager().SetTimer(StatusUpdateTimer, this, &UAcCharacterStatusComponent::UpdateCharacterStatus, .5f, true);
}

void UAcCharacterStatusComponent::UpdateCharacterStatus()
{
	if (!AbilitySystemComponent || !CombatAttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterStatusComponent: Failed to get ASC or AttributeSet"));
	}

	float Stamina = CombatAttributeSet->GetStamina();
	
	/*if (GEngine && CombatAttributeSet)
	{

		GEngine->AddOnScreenDebugMessage(
			-1,                         // Key: replaces existing message with this key
			5.f,                       // Time to display (0 = one frame)
			FColor::Green,             // Text color
			FString::Printf(TEXT("Stamina: %.2f"), Stamina)
		);
	}*/
	
}



