// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "HauntedHouseGameInstance.h"
#include "LobbyGameState.h"


void ALobbyGameMode::SetPlayerDefaults(APawn* PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);

	const auto InGamePlayerState = Cast<AInGamePlayerState>(PlayerPawn->GetPlayerState());
	const auto LobbyGameState = GetGameState<ALobbyGameState>();
	
	if(InGamePlayerState != nullptr && LobbyGameState != nullptr)
	{
		LobbyGameState->AssignPlayerNextAvailableCharacter(InGamePlayerState);
	}
}
