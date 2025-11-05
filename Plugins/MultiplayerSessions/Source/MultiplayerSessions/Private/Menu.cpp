// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

bool UMenu::Initialize()
{
	if(! Super::Initialize())
	{
		return false;
	}

	if(HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	if(JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

void UMenu::MenuSetup(int32 numPublicConnections, FString matchType, FString lobbyPath)
{
	NumPublicConnections = numPublicConnections;
	MatchType = matchType;
	PathToLobby = FString::Printf(TEXT("%s?listen"), *lobbyPath);
	
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if(World != nullptr)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if(PlayerController != nullptr)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if(GameInstance != nullptr)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance is not valid!"));
	}

	if(MultiplayerSessionsSubsystem != nullptr)
	{
		MultiplayerSessionsSubsystem->MMSOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MMSOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MMSOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MMSOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MMSOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MultiplayerSessionsSubsystem is not valid. Cannot hookup listener events"));
	}
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Log, TEXT("OnCreateSession!"));
	if(bWasSuccessful)
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Cyan,
				FString(TEXT("Session created successfully!"))
				);
		}

		UWorld* World = GetWorld();
		if(World != nullptr)
		{
			World->ServerTravel(PathToLobby);
		}
	}
	else
	{
		HostButton->SetIsEnabled(true);
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Session created failed!"))
				);
		}
	}

	if(MultiplayerSessionsSubsystem != nullptr)
	{
		MultiplayerSessionsSubsystem->MMSOnCreateSessionComplete.Clear();
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionsFound, bool bWasSuccessful)
{
	if(MultiplayerSessionsSubsystem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot join match because MultiplayerSessionsSubsystem is nullptr"));
		JoinButton->SetIsEnabled(true);
		return;
	}
	
	for(FOnlineSessionSearchResult result : SessionsFound)
	{
		FString SessionMatchType;
		result.Session.SessionSettings.Get(FName("MatchType"), SessionMatchType);
		if(SessionMatchType == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(result);
		}
	}
	if(!bWasSuccessful || SessionsFound.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Log, TEXT("OnJoinSession!"));
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if(Subsystem != nullptr)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

		if(SessionInterface.IsValid())
		{
			FString Address;
			if (SessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
			{
				APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();
				if(playerController != nullptr)
				{
					UE_LOG(LogTemp, Log, TEXT("Traveling to address: %s"), *Address);
					playerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to get ResolvedConnectString!"));
				MultiplayerSessionsSubsystem->DestroySession();
			}
		}
	}

	if(Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
	HostButton->SetIsEnabled(true);
	JoinButton->SetIsEnabled(true);
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("HostButtonClicked!"));
	HostButton->SetIsEnabled(false);
	if(MultiplayerSessionsSubsystem != nullptr)
	{
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot create session because MultiplayerSessionsSubsystem is not valid!"));
	}
}

void UMenu::JoinButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("JoinButtonClicked!"));
	JoinButton->SetIsEnabled(false);
	if(MultiplayerSessionsSubsystem != nullptr)
	{
		MultiplayerSessionsSubsystem->FindSessions(10);
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if(World != nullptr)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if(PlayerController != nullptr)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}
