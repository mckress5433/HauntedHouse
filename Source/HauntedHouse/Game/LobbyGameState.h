// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LobbyGameState.generated.h"


USTRUCT(BlueprintType)
struct FCharacterSelectionDatum
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBaseCharacterDataAsset* CharacterDataAsset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AInGamePlayerState* PlayerState;
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayersCharacterInfoUpdated);

UCLASS()
class HAUNTEDHOUSE_API ALobbyGameState : public AGameStateBase
{
	GENERATED_BODY()


	UPROPERTY(EditDefaultsOnly, Replicated, ReplicatedUsing=OnRep_CharacterSelectionData)
	TArray<FCharacterSelectionDatum> CharacterSelectionData;
	
	UPROPERTY(BlueprintAssignable)
	FOnPlayersCharacterInfoUpdated OnPlayersCharacterInfoUpdatedDelegate;

protected:
	UFUNCTION()
	void OnRep_CharacterSelectionData();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FCharacterSelectionDatum> GetPlayerCharctersInfos() { return CharacterSelectionData; }

public:
	void AssignPlayerNextAvailableCharacter(AInGamePlayerState* PlayerState);
	
	void UpdatePlayerCharactersInfos(AInGamePlayerState* PlayerState, const UBaseCharacterDataAsset* NewCharacterDataAsset);
};
