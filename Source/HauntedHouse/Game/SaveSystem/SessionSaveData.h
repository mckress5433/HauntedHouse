// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "HauntedHouse/Global/GlobalStructs.h"
#include "SessionSaveData.generated.h"

/**
 * 
 */
UCLASS()
class HAUNTEDHOUSE_API USessionSaveData : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FSessionSaveStruct SaveData;
};
