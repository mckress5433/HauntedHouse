// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTransitionTrigger.h"

#include "HauntedHouse/HauntedHouse.h"
#include "HauntedHouse/Game/HauntedHouseGameMode.h"
#include "HauntedHouse/Game/SaveSystem/SaveGameSubsystem.h"
#include "HauntedHouse/Global/GlobalFunctionLibrary.h"
#include "HauntedHouse/Player/InGamePlayerController.h"
#include "HauntedHouse/Player/PlayerState/InGamePlayerState.h"

// Sets default values
ALevelTransitionTrigger::ALevelTransitionTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	InteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Interactable Mesh"));
	RootComponent = InteractableMesh;
	
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(FName("InteractableComponent"));
}

void ALevelTransitionTrigger::OnInteractEvent()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Multicast_TiggerSaveOnClients();
		UE_LOG(LogSaveGame, Log, TEXT("Level transition trigger"));
	
		if(const UWorld* world = GetWorld(); world != nullptr)
		{
			if(const auto gameMode = Cast<AHauntedHouseGameMode>(world->GetAuthGameMode()); gameMode != nullptr)
			{
				gameMode->InitiateServerTravel("/Game/Maps/HauntedHouse", true);
			}
		}
	}
}

void ALevelTransitionTrigger::BeginPlay()
{
	Super::BeginPlay();

	if(GetLocalRole() == ROLE_Authority && InteractableComponent != nullptr)
	{
		InteractableComponent->OnInteractEvent.AddDynamic(this, &ALevelTransitionTrigger::OnInteractEvent);
	}
}

void ALevelTransitionTrigger::Multicast_TiggerSaveOnClients_Implementation()
{
	UGameInstance* gameInstance = GetGameInstance();
	UWorld* world = GetWorld();
	if(gameInstance != nullptr && world != nullptr)
	{
		auto saveSystem = gameInstance->GetSubsystem<USaveGameSubsystem>();
		auto playerController = static_cast<AInGamePlayerController*>(world->GetFirstPlayerController());
		if (saveSystem != nullptr && playerController != nullptr)
		{
			if (auto playerState = playerController->GetPlayerState<AInGamePlayerState>(); playerState != nullptr)
			{
				if (GlobalFunctionLibrary::GetSaveSystemDebugValue() != 0)
				{
					playerState->PrintSessionData();
				}
				
				FSessionSaveStruct SessionSaveData = FSessionSaveStruct(playerState->GetCharacterInfo(), 0);
				saveSystem->SaveSessionSaveData(SessionSaveData, [this](bool bWasSuccessful)
				{
					if (bWasSuccessful)
					{
						
					}
				});	
			}
		}
	}
}


