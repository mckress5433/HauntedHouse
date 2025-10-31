// Copyright Epic Games, Inc. All Rights Reserved.

#include "HauntedHouseGameMode.h"
#include "../Character/BaseCharacter.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "UObject/ConstructorHelpers.h"

AHauntedHouseGameMode::AHauntedHouseGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	bUseSeamlessTravel = true;
}

void AHauntedHouseGameMode::InitiateServerTravel(const FString& MapName, bool bIsSeamlessTravel)
{
	// Check if we are the server (only the server can initiate server travel)
	if (HasAuthority())
	{
		// Set seamless travel based on the input parameter
		GetWorld()->ServerTravel(MapName, bIsSeamlessTravel);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Server travel can only be initiated by the server."));
	}
}

void AHauntedHouseGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	if(GameState != nullptr)
	{
		int32 numberOfPlayers = GameState.Get()->PlayerArray.Num();

		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				1,
				60.f,
				FColor::Cyan,
				FString::Printf(TEXT("Players in game: %d"), numberOfPlayers));
			
			APlayerState* playerState = NewPlayer->GetPlayerState<APlayerState>();

			if(playerState != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(
					1,
					60.f,
					FColor::Cyan,
					FString::Printf(TEXT("%s has joined the game!"), *playerState->GetPlayerName()));
			}
		}
	}
}

void AHauntedHouseGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if(GameState != nullptr)
	{
		int32 numberOfPlayers = GameState.Get()->PlayerArray.Num();

		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				1,
				60.f,
				FColor::Cyan,
				FString::Printf(TEXT("Players in game: %d"), numberOfPlayers));
			
			APlayerState* playerState = Exiting->GetPlayerState<APlayerState>();

			if(playerState != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(
					1,
					60.f,
					FColor::Cyan,
					FString::Printf(TEXT("%s has left the game!"), *playerState->GetPlayerName()));
			}
		}
	}
}