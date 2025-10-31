// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HauntedHouseGameMode.generated.h"

UCLASS(minimalapi)
class AHauntedHouseGameMode : public AGameModeBase
{
	GENERATED_BODY()

	void OnPostLogin(AController* NewPlayer) override;
	void Logout(AController* Exiting) override;
public:
	AHauntedHouseGameMode();

	// Function to initiate server travel
	UFUNCTION(BlueprintCallable, Category = "ServerTravel")
	void InitiateServerTravel(const FString& MapName, bool bIsSeamlessTravel = true);
};



