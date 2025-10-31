// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelTransitionTirgger.h"

#include "GameFramework/GameMode.h"
#include "HauntedHouse/Game/HauntedHouseGameMode.h"

// Sets default values
ALevelTransitionTirgger::ALevelTransitionTirgger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Interactable Mesh"));
	RootComponent = InteractableMesh;
	
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(FName("InteractableComponent"));
	InteractableComponent->SetupAttachment(InteractableMesh);
}

void ALevelTransitionTirgger::OnInteractEvent()
{
	if(const UWorld* world = GetWorld(); world != nullptr)
	{
		if(const auto gameMode = Cast<AHauntedHouseGameMode>(world->GetAuthGameMode()); gameMode != nullptr)
		{
			gameMode->InitiateServerTravel("/Game/Maps/HauntedHouse", true);
		}
	}
	
}

void ALevelTransitionTirgger::BeginPlay()
{
	Super::BeginPlay();

	if(GetLocalRole() == ROLE_Authority && InteractableComponent != nullptr)
	{
		InteractableComponent->OnInteract.AddDynamic(this, &ALevelTransitionTirgger::OnInteractEvent);
	}
}


