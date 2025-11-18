// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterDataAsset.h"

#include "Net/UnrealNetwork.h"

FPlayersCharacterInfo UBaseCharacterDataAsset::ConvertToPlayersCharacterInfo() const
{
	return FPlayersCharacterInfo(CharacterColor, CharacterMeshData, CharacterAttributeData);
}

void UBaseCharacterDataAsset::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CharacterColor);
	DOREPLIFETIME(ThisClass, CharacterMeshData);
	DOREPLIFETIME(ThisClass, CharacterAttributeData);
}
