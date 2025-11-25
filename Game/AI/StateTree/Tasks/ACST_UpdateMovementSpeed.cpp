// Fill out your copyright notice in the Description page of Project Settings.


#include "dazAda/Game/AI/StateTree/Tasks/ACST_UpdateMovementSpeed.h"
#include "dazAda//Game/AI/AC_AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "dazAda/Game/Characters/AcBaseCharacter.h"

EStateTreeRunStatus UACST_UpdateMovementSpeed::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);

	// Safety checks
	if (!Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("PGAS_GetRandomLocation: Actor is null."));
		return EStateTreeRunStatus::Failed;
	}

	// Get the character movement component
	if (UCharacterMovementComponent* MoveComp = Actor->GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = TargetMaxWalkSpeed; // Set the desired MaxWalkSpeed
		return EStateTreeRunStatus::Succeeded;   // Successfully set the speed
	}

	UE_LOG(LogTemp, Warning, TEXT("PGAS_UpdateMovementSpeed: Character or MovementComponent not valid"));
	return EStateTreeRunStatus::Failed;
}

#if WITH_EDITOR
FText UACST_UpdateMovementSpeed::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	return FText::Format(
		NSLOCTEXT("PGAS", "UpdateMovementSpeed", "Set Actor Max Walk Speed to {0}"),
		FText::AsNumber(TargetMaxWalkSpeed)
	);
}
#endif /*WITH_EDITOR*/