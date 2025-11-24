// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "EnhancedInputSubsystems.h"
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

protected:
	// Input Mapping Context and Input Actions
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	// Input Mapping Context for in editor
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* EditorMappingContext;
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction>  UIInteractionAction;

private:
	UPROPERTY(EditDefaultsOnly)
	FString LobbyLevelPath = "/Game/Maps/Lobby";

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;

protected:
	virtual void BeginPlay() override;
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	virtual void OnUIInteraction();

public:
	virtual void SetupInputComponent() override;
};
