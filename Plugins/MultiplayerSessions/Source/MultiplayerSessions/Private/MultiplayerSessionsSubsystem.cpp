// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "TransformNoScale.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Online/OnlineSessionNames.h"

DEFINE_LOG_CATEGORY(LogSessionsSubsystem)

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem():
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if(Subsystem != nullptr)
	{
		SessionInterface = Subsystem->GetSessionInterface();
	}
}

void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if(!SessionInterface.IsValid())
	{
		UE_LOG(LogSessionsSubsystem, Error, TEXT("Failed to create session because SessionInterface is invalid"));
		MMSOnCreateSessionComplete.Broadcast(false);
		return;
	}

	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if(ExistingSession != nullptr)
	{
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastMatchType = MatchType;
		DestroySession();
		return;
	}

	// Store the delegate in an FDelegateHandle so we can later remove it from a delegate list
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	LastSessionSettings = MakeShareable(new FOnlineSessionSettings);
	LastSessionSettings->bIsLANMatch = false;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->BuildUniqueId = 1;

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (LocalPlayer != nullptr)
	{
		const FUniqueNetIdRepl UserId = *LocalPlayer->GetPreferredUniqueNetId();
		if (UserId.IsValid())
		{
			if(!SessionInterface->CreateSession(*UserId, NAME_GameSession, *LastSessionSettings))
			{
				UE_LOG(LogSessionsSubsystem, Error, TEXT("Failed to create session for unknown reason"));
				MMSOnCreateSessionComplete.Broadcast(false);
				SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
			}
		}
		else
		{
			UE_LOG(LogSessionsSubsystem, Error, TEXT("Cannot create session because UserId is invalid"));
		}
	}
	else
	{
		UE_LOG(LogSessionsSubsystem, Error, TEXT("Cannot create session because local player is invalid"));
	}
}

void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	if(!SessionInterface.IsValid())
	{
		UE_LOG(LogSessionsSubsystem, Error, TEXT("Failed to find sessions because SessionInterface is invalid"));
		MMSOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	FindSessionCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if(!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		UE_LOG(LogSessionsSubsystem, Error, TEXT("Failed to find sessions for unkown reason"));
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegateHandle);
		MMSOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if(!SessionInterface.IsValid())
	{
		UE_LOG(LogSessionsSubsystem, Error, TEXT("Failed to join session because session interface is invalid"));
		MMSOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	UE_LOG(LogSessionsSubsystem, Log, TEXT("Joining session..."));

	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if(!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		UE_LOG(LogSessionsSubsystem, Error, TEXT("Failed to join session for unknown reason"));
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		MMSOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
	
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
	if(!SessionInterface.IsValid())
	{
		UE_LOG(LogSessionsSubsystem, Error, TEXT("Cannot destroy session because SessionInterface is invalid!"));
		MMSOnDestroySessionComplete.Broadcast(false);
		return;
	}

	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if(ExistingSession != nullptr)
	{
		DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);
		if(!SessionInterface->DestroySession(NAME_GameSession))
		{
			UE_LOG(LogSessionsSubsystem, Error, TEXT("Failed to destroy session for unknown reason"));
			SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
			MMSOnDestroySessionComplete.Broadcast(false);
		}
	}
	else
	{
		UE_LOG(LogSessionsSubsystem, Warning, TEXT("Failed to destroy session because session does not exist"));
	}
}

void UMultiplayerSessionsSubsystem::StartSession()
{
	if(SessionInterface != nullptr)
	{
		
	}
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if(SessionInterface != nullptr)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
		
		if(bWasSuccessful)
		{
			UE_LOG(LogSessionsSubsystem, Log, TEXT("Session %s was successfully created!"), *SessionName.ToString());
			
			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			if(LocalPlayer != nullptr)
			{
				const FUniqueNetIdRepl UserId = LocalPlayer->GetPreferredUniqueNetId();
				if (UserId.IsValid())
				{
					SessionInterface->RegisterPlayer(SessionName, *UserId, false);
				}
				else
				{
					UE_LOG(LogSessionsSubsystem, Error, TEXT("Cannot register player because UserId is invalid"));
				}
			}
			else
			{
				UE_LOG(LogSessionsSubsystem, Error, TEXT("Cannot register player because local player is not valid"));
			}
		}
	}

	if(!bWasSuccessful)
	{
		UE_LOG(LogSessionsSubsystem, Error, TEXT("Failed to create session %s"), *SessionName.ToString());
	}
	
	MMSOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if(SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegateHandle);
	}

	if(LastSessionSearch->SearchResults.Num() <= 0)
	{
		UE_LOG(LogSessionsSubsystem, Warning, TEXT("Failed to find sessions. Session list is empty"));
		MMSOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
	else
	{
		UE_LOG(LogSessionsSubsystem, Log, TEXT("Found %d sessions!"), LastSessionSearch->SearchResults.Num());
		MMSOnFindSessionsComplete.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
	}
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(SessionInterface != nullptr)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	if(Result == EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogSessionsSubsystem, Log, TEXT("Session %s was successfully joined!"), *SessionName.ToString());
	}
	else
	{
		UE_LOG(LogSessionsSubsystem, Error, TEXT("Failed to join session %s (error: %i)"), *SessionName.ToString(), Result);
	}
	MMSOnJoinSessionComplete.Broadcast(Result);
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if(SessionInterface != nullptr)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	if(bWasSuccessful)
	{
		UE_LOG(LogSessionsSubsystem, Log, TEXT("Session %s was successfully destroyed"), *SessionName.ToString());
		if(bCreateSessionOnDestroy)
		{
			CreateSession(LastNumPublicConnections, LastMatchType);
		}
	}
	else
	{
		UE_LOG(LogSessionsSubsystem, Log, TEXT("Failed to destroy session %s"), *SessionName.ToString());
	}
	
	MMSOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
}

void UMultiplayerSessionsSubsystem::OnSessionUserInviteAccepted(int32 LocalUserNum, bool bWasSuccessful,
	const FOnlineSessionSearchResult& SearchResult)
{
}
