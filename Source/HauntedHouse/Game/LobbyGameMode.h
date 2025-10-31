// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HauntedHouseGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class HAUNTEDHOUSE_API ALobbyGameMode : public AHauntedHouseGameMode
{
	GENERATED_BODY()

	virtual void SetPlayerDefaults(APawn* PlayerPawn) override;
};
