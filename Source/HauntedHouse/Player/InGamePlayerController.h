// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerController.h"
#include "InGamePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class HAUNTEDHOUSE_API AInGamePlayerController : public ABasePlayerController
{
	GENERATED_BODY()

	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* InteractAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* ToggleOptionsMenuAction;

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
	
	virtual void OnUIInteraction() override;

	UFUNCTION(BlueprintCallable)
	void SetUIInputMode();
	UFUNCTION(BlueprintCallable)
	void SetGameInputMode();

public:
	
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintPure)
	bool IsPauseMenuOpen() const;
};
