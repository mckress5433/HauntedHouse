// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGameSubsystem.h"
#include "SessionSaveData.h"
#include "HauntedHouse/HauntedHouse.h"
#include "HauntedHouse/Global/GlobalFunctionLibrary.h"

void USaveGameSubsystem::SaveSessionSaveData(FSessionSaveStruct SaveData, FOnSessionDataSaved Callback)
{
	USessionSaveData* SaveGameObject = Cast<USessionSaveData>(CreateSessionSaveData());

	SaveGameObject->SaveData = SaveData;

	PendingSessionSavedCallback = MoveTemp(Callback);

	ensureMsgf(!SaveDelegate.IsBound(), TEXT("SaveDelegate is already bound."));

	if (GlobalFunctionLibrary::GetSaveSystemDebugValue() !=0)
	{
		UE_LOG(LogSaveGame, Log, TEXT("Saving SessionData..."));
	}

	SaveDelegate.BindUObject(this, &USaveGameSubsystem::OnSaveGameComplete);
	UGameplayStatics::SaveGameToSlot(CachedSessionSaveData, SessionSlotName, SLOT_INDEX);
}

void USaveGameSubsystem::OnSaveGameComplete(const FString& SlotName, int32 UserIndex, bool bSuccess)
{
	ensureMsgf(bSuccess, TEXT("Failed to save SesssionData"));
	PendingSessionSavedCallback(bSuccess);
}

void USaveGameSubsystem::LoadOrCreateSessionSaveData(FOnSessionDataAcquired Callback)
{
	// Store contents of lambda function
	PendingSessionLoadedCallback = MoveTemp(Callback);

	if (UGameplayStatics::DoesSaveGameExist(SESSION_SLOT_NAME, SLOT_INDEX))
	{
		LoadSessionSaveData();
	}
	else
	{
		CreateSessionSaveData();
		PendingSessionLoadedCallback(CachedSessionSaveData->SaveData);
	}
}

USessionSaveData* USaveGameSubsystem::CreateSessionSaveData()
{
	CachedSessionSaveData = Cast<USessionSaveData>(UGameplayStatics::CreateSaveGameObject(USessionSaveData::StaticClass()));
	ensureMsgf(CachedSessionSaveData != nullptr, TEXT("Failed to create SessionSaveData"));
	if (GlobalFunctionLibrary::GetSaveSystemDebugValue() != 0)
	{
		UE_LOG(LogSaveGame, Log, TEXT("Session data created!"));
	}

	return CachedSessionSaveData;
}

void USaveGameSubsystem::LoadSessionSaveData()
{
	ensureMsgf(!LoadDelegate.IsBound(), TEXT("LoadDelegate is already bound."));

	if (GlobalFunctionLibrary::GetSaveSystemDebugValue() != 0)
	{
		UE_LOG(LogSaveGame, Log, TEXT("Starting session load..."));
	}
	
	LoadDelegate.BindUObject(this, &USaveGameSubsystem::OnLoadSessionSaveData_Complete);
	UGameplayStatics::AsyncLoadGameFromSlot(SESSION_SLOT_NAME, SLOT_INDEX, LoadDelegate);
}

void USaveGameSubsystem::OnLoadSessionSaveData_Complete(const FString& SlotName, int32 UserIndex, USaveGame* LoadedGame)
{
	LoadDelegate.Unbind();
	
	CachedSessionSaveData = Cast<USessionSaveData>(LoadedGame);
	ensureMsgf(CachedSessionSaveData != nullptr, TEXT("Failed to load session save data! SessionSaveData is null"));

	if (GlobalFunctionLibrary::GetSaveSystemDebugValue() != 0)
	{
		UE_LOG(LogSaveGame, Log, TEXT("Session data loaded!"));
	}

	PendingSessionLoadedCallback(CachedSessionSaveData->SaveData);
	
}


