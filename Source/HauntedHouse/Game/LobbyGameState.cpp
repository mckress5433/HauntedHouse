// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameState.h"

#include "HauntedHouse/Player/PlayerState/InGamePlayerState.h"
#include "Net/UnrealNetwork.h"

void ALobbyGameState::OnRep_CharacterSelectionData()
{
	OnPlayersCharacterInfoUpdatedDelegate.Broadcast();

	for(int i=0; i<CharacterSelectionData.Num(); i++)
	{
		if(AInGamePlayerState* PlayerState = CharacterSelectionData[i].PlayerState; PlayerState != nullptr)
		{
			PlayerState->UpdateCharacterInfoAndMeshes(CharacterSelectionData[i].CharacterDataAsset->ConvertToPlayersCharacterInfo());
		}
	}
}

void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CharacterSelectionData);
}

void ALobbyGameState::AssignPlayerNextAvailableCharacter(AInGamePlayerState* PlayerState)
{
	if(PlayerState != nullptr && GetLocalRole() != ROLE_Authority) return;

	// Have to iterate for pointer to be valid apparently 
	for(int i=0; i<CharacterSelectionData.Num(); i++)
	{
		if(CharacterSelectionData[i].PlayerState == nullptr)
		{
			CharacterSelectionData[i].PlayerState = PlayerState;
			FPlayersCharacterInfo CharacterInfo = CharacterSelectionData[i].CharacterDataAsset->ConvertToPlayersCharacterInfo();
			CharacterSelectionData[i].PlayerState->UpdateCharacterInfoAndMeshes(CharacterInfo);
			break;
		}
	}
	OnPlayersCharacterInfoUpdatedDelegate.Broadcast();
}

void ALobbyGameState::UpdatePlayerCharactersInfos(AInGamePlayerState* PlayerState,
	const UBaseCharacterDataAsset* NewCharacterDataAsset)
{
	TArray<FCharacterSelectionDatum> tmpArray = CharacterSelectionData;
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

	CharacterSelectionData = tmpArray;
	OnPlayersCharacterInfoUpdatedDelegate.Broadcast();
	PlayerState->UpdateCharacterInfoAndMeshes(NewCharacterDataAsset->ConvertToPlayersCharacterInfo());
}
