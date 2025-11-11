// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BasePlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class HAUNTEDHOUSE_API UBasePlayerHUD : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeOnInitialized() override;

protected:
	UFUNCTION(BlueprintNativeEvent)
	void ShowInteractionPrompt();
	UFUNCTION(BlueprintNativeEvent)
	void HideInteractionPrompt();
	UFUNCTION(BlueprintNativeEvent)
	void StartInteraction();
	UFUNCTION(BlueprintNativeEvent)
	void EndInteraction();
	UFUNCTION(BlueprintNativeEvent)
	void UpdateInteractionProgress(uint8 ProgressPercentage);
	
};
