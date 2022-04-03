// Copyright Epic Games, Inc. All Rights Reserved.

#include "HauntedHouseGameMode.h"
#include "HauntedHouseCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHauntedHouseGameMode::AHauntedHouseGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
