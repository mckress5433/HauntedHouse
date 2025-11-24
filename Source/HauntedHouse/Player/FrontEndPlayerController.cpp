// Fill out your copyright notice in the Description page of Project Settings.


#include "FrontEndPlayerController.h"

#include "HauntedHouse/Character/BaseCharacter.h"

void AFrontEndPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Show the mouse cursor
	bShowMouseCursor = true;
	// Set input mode to Game and UI (this allows keyboard inputs to affect the game while interacting with UI)
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
	// Disable camera control while interacting with the UI
	SetIgnoreLookInput(true);
}
