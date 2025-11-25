// Copyright Epic Games, Inc. All Rights Reserved.

#include "dazAdaGameMode.h"
#include "dazAdaCharacter.h"
#include "UObject/ConstructorHelpers.h"

AdazAdaGameMode::AdazAdaGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
