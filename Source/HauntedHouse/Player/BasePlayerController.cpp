// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Online/OnlineSessionNames.h"

ABasePlayerController::ABasePlayerController():
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete))
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if(OnlineSubsystem != nullptr)
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
	}
}

void ABasePlayerController::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Blue,
			FString::Printf(TEXT("Created session: %s"), *SessionName.ToString())
		);

		UWorld* world = GetWorld();
		if(world != nullptr)
		{
			world->ServerTravel(FString(LobbyLevelPath + "?listen"));
		}
	}
	else
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Magenta,
				FString(TEXT("Failed to create session!"))
			);
		}
	}
}

void ABasePlayerController::OnFindSessionsComplete(bool bWasSuccessful)
{
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Cyan,
			FString::Printf(TEXT("Displaying sessions...")));
	}

	for(FOnlineSessionSearchResult result : SessionSearch->SearchResults)
	{
		FString Id = result.GetSessionIdStr();
		FString User = result.Session.OwningUserName;
		
		if(GEngine != nullptr)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Cyan,
				FString::Printf(TEXT("Id: %s, User: %s"), *Id, *User)
			);
		}

		FString MatchType;
		result.Session.SessionSettings.Get(FName("MatchType"), MatchType);

		if(MatchType == FString("StatusPro"))
		{
			if(GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Cyan,
					FString::Printf(TEXT("Found Free for All matchtype")));
			}
			
			OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

			if(ULocalPlayer* LocalPlayer = GetLocalPlayer(); LocalPlayer != nullptr)
			{
				OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, result);
			}
		}
	}
}

void ABasePlayerController::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(!OnlineSessionInterface.IsValid()) return;

	FString Address;
	if(OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Magenta,
				FString::Printf(TEXT("Connect string: %s"), *Address)
				);
		}
	}

	APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();
	if(playerController != nullptr)
	{
		playerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}
