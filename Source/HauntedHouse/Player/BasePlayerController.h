// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HAUNTEDHOUSE_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	
	ABasePlayerController();
	
	//Pointer to the online session interface
	IOnlineSessionPtr OnlineSessionInterface;

private:
	UPROPERTY(EditDefaultsOnly)
	FString LobbyLevelPath = "/Game/Maps/Lobby";

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;

protected:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

};
