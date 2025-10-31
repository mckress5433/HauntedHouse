// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameState.h"

#include "HauntedHouse/Player/PlayerState/InGamePlayerState.h"
#include "Net/UnrealNetwork.h"

void ALobbyGameState::UpdatePlayersCharacterInfos(AInGamePlayerState* PlayerState,
	UBaseCharacterDataAsset* NewCharacterDataAsset)
{
	TArray<FPlayerCharactersInfo> tmpArray = PlayersCharacterInfos;
	// Have to iterate for pointer to be valid apparently 
	for(int i=0; i<tmpArray.Num(); i++)
	{
		if(tmpArray[i].PlayerState == PlayerState)
		{
			tmpArray[i].PlayerState = nullptr;
		}

		if(tmpArray[i].CharacterDataAsset == NewCharacterDataAsset)
		{
			tmpArray[i].PlayerState = PlayerState;
		}
	}

	PlayersCharacterInfos = tmpArray;
	OnPlayersCharacterInfoUpdatedDelegate.Broadcast();
	PlayerState->UpdateCharacterInfoAndMeshes(NewCharacterDataAsset);
}

void ALobbyGameState::OnRep_PlayersCharacterInfos()
{
	OnPlayersCharacterInfoUpdatedDelegate.Broadcast();

	for(int i=0; i<PlayersCharacterInfos.Num(); i++)
	{
		if(AInGamePlayerState* PlayerState = PlayersCharacterInfos[i].PlayerState; PlayerState != nullptr)
		{
			PlayerState->UpdateCharacterInfoAndMeshes(PlayersCharacterInfos[i].CharacterDataAsset);
		}
	}
}

void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayersCharacterInfos);
}

void ALobbyGameState::AssignPlayerNextAvailableCharacter(AInGamePlayerState* PlayerState)
{
	if(PlayerState == nullptr) return;

	// Have to iterate for pointer to be valid apparently 
	for(int i=0; i<PlayersCharacterInfos.Num(); i++)
	{
		if(PlayersCharacterInfos[i].PlayerState == nullptr)
		{
			PlayersCharacterInfos[i].PlayerState = PlayerState;
			PlayersCharacterInfos[i].PlayerState->UpdateDefaultCharacterDataAsset(PlayersCharacterInfos[i].CharacterDataAsset);
			break;
		}
	}
	OnPlayersCharacterInfoUpdatedDelegate.Broadcast();
}

void ALobbyGameState::UpdatePlayerCharactersInfos(AInGamePlayerState* PlayerState,
	UBaseCharacterDataAsset* NewCharacterDataAsset)
{
	UpdatePlayersCharacterInfos(PlayerState, NewCharacterDataAsset);
}
