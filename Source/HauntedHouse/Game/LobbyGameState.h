// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LobbyGameState.generated.h"


USTRUCT(BlueprintType)
struct FPlayerCharactersInfo
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


	UPROPERTY(EditDefaultsOnly, Replicated, ReplicatedUsing=OnRep_PlayersCharacterInfos)
	TArray<FPlayerCharactersInfo> PlayersCharacterInfos;
	
	UPROPERTY(BlueprintAssignable)
	FOnPlayersCharacterInfoUpdated OnPlayersCharacterInfoUpdatedDelegate;

protected:
	void UpdatePlayersCharacterInfos(AInGamePlayerState* PlayerState, UBaseCharacterDataAsset* NewCharacterDataAsset);

	UFUNCTION()
	void OnRep_PlayersCharacterInfos();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FPlayerCharactersInfo> GetPlayerCharctersInfos() { return PlayersCharacterInfos; }

public:
	void AssignPlayerNextAvailableCharacter(AInGamePlayerState* PlayerState);

	UFUNCTION()
	void UpdatePlayerCharactersInfos(AInGamePlayerState* PlayerState, UBaseCharacterDataAsset* NewCharacterDataAsset);
};
