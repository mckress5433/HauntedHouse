// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HauntedHouse/Global/GlobalStructs.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveGameSubsystem.generated.h"

class USessionSaveData;

const FString SESSION_SLOT_NAME = "SessionData";
constexpr int32 SLOT_INDEX = 0;

UCLASS()
class HAUNTEDHOUSE_API USaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	using FOnSessionDataAcquired = TFunction<void(FSessionSaveStruct SaveData)>;
	using FOnSessionDataSaved = TFunction<void(bool bWasSuccessful)>;

	void SaveSessionSaveData(FSessionSaveStruct SaveData, FOnSessionDataSaved Callback);
	void LoadOrCreateSessionSaveData(FOnSessionDataAcquired Callback);
	
private:

	FOnSessionDataAcquired PendingSessionLoadedCallback;
	FOnSessionDataSaved PendingSessionSavedCallback;
	
	UPROPERTY()
	TObjectPtr<USessionSaveData> CachedSessionSaveData = nullptr;
	FString SessionSlotName;
	int32 SessionUserIndex;
	
	FAsyncLoadGameFromSlotDelegate LoadDelegate;
	FAsyncSaveGameToSlotDelegate SaveDelegate;
	
	const USessionSaveData* GetSessionSaveData() { return CachedSessionSaveData;}

	void OnSaveGameComplete(const FString& SlotName, int32 UserIndex, bool bSuccess);
	USessionSaveData* CreateSessionSaveData();
	void LoadSessionSaveData();
	void OnLoadSessionSaveData_Complete(const FString& SlotName, int32 UserIndex, USaveGame* LoadedGame);
};
