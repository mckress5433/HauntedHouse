// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameGameMode.h"
#include "HauntedHouse/Player/PlayerState/InGamePlayerState.h"

void AInGameGameMode::SetPlayerDefaults(APawn* PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);

	if (const auto InGamePlayerState = Cast<AInGamePlayerState>(PlayerPawn->GetPlayerState()); InGamePlayerState != nullptr)
	{
		InGamePlayerState->Client_LoadPlayerData();
	}
	
}
