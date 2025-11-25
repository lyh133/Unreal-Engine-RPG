// Fill out your copyright notice in the Description page of Project Settings.


#include "AcNPC.h"
#include "dazAda/Game/AbilitySystem/AcAbilitySystemComponent.h"
#include "dazAda/Game/AI/AC_AIController.h"
#include "dazAda/Game/AbilitySystem/Attributes/AcCombatAttributeSet.h"
// Sets default values
AAcNPC::AAcNPC()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AbilitySystemComponent = CreateDefaultSubobject<UAcAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

// Called when the game starts or when spawned
void AAcNPC::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	GiveDefaultAbilities();
	InitDefaultAbilities();
	InitStartupEffects();
}

// Called every frame
void AAcNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AAcNPC::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Set a 0.2-second delay
	FTimerHandle DummyHandle;
	GetWorld()->GetTimerManager().SetTimer(
		DummyHandle,
		[ this, NewController ] ()
		{
			// Cast controller to our custom AI controller
			if (AAC_AIController* EnemyAIController = Cast<AAC_AIController>(NewController))
			{
				if (EnemyAIController->GetStateTreeAIComponent() && StateTree)
				{
					EnemyAIController->GetStateTreeAIComponent()->StartStateTree(StateTree);
				}
			}
		},
		0.2f, // Delay time
		false // No repeat
	);
}
void AAcNPC::UnPossessed()
{
	Super::UnPossessed();
}

void AAcNPC::OnDeath()
{
	Super::OnDeath();
	
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->StopMovement();
		AICon->GetBrainComponent()->StopLogic(TEXT("Character died"));
	}

	// Optional NPC death cleanup
	SetLifeSpan(5.0f);
}
