// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HauntedHouse/Widgets/BaseWidget.h"
#include "PlayerState/InGamePlayerState.h"
#include "InGamePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HAUNTEDHOUSE_API AInGamePlayerController : public ABasePlayerController
{
	GENERATED_BODY()

	
protected:
	// Input Mapping Context and Input Actions
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	// Input Mapping Context for in editor
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* EditorMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* InteractAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ToggleOptionsMenuAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* UIInteractionAction;

public:
	
/*
 * Functions
 */
private:

protected:
	virtual void BeginPlay() override;

	// Movement Input Handling
	void Move(const FInputActionValue& Value);
	// Look Input Handling (for camera control)
	void Look(const FInputActionValue& Value);
	// Event triggered when the interaction input is pressed/released
	void StartInteraction();
	void EndInteraction();
	// Event to display pause menu
	void ToggleOptionsMenu();

	void OnUIInteraction();

	UFUNCTION(BlueprintCallable)
	void SetUIInputMode();
	UFUNCTION(BlueprintCallable)
	void SetGameInputMode();
	
public:
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintPure)
	bool IsPauseMenuOpen() const;
};
