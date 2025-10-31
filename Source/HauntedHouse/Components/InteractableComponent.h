// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InteractableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteract);
UCLASS(Blueprintable, ClassGroup="HauntedHouse", hidecategories=(Object,Activation,"Components|Activation",Sockets,Base,Lighting,LOD,Mesh), editinlinenew, meta=(BlueprintSpawnableComponent))
class HAUNTEDHOUSE_API UInteractableComponent : public UWidgetComponent
{
	GENERATED_BODY()
	UInteractableComponent();
	
	FTimerHandle InteractionTimerHandle;
	float CurrentHoldTime;
	float bHasBeenTriggered = false;
	// Basically whether this is showing interaction text
	bool bIsActive = false;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	bool bHostOnly = false;
	// The amount of time the player must hold the interaction button to complete the interaction
	UPROPERTY(EditDefaultsOnly)
	float HoldTime = 0.0f;
	// The tick rate in seconds for the interaction tick
	UPROPERTY(EditDefaultsOnly)
	float InteractionDeltaTIme = 0.06f;

	UPROPERTY(EditDefaultsOnly)
	bool bOnlyInteractOnce = true;
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnInteract OnInteract;
	
	/*
	 * Functions
	 */
private:
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	void InteractionTick();
	void EndInteraction();
	
public:
	void Interact();
	void CancelInteraction();

	// Should only be called locally
	void ShowInteractionText();
	void HideInteractionText();

	bool CanInteract() const { return !bHasBeenTriggered; }
	bool GetIsActive() const { return bIsActive; }
	bool GetIsHostOnly() const { return bHostOnly; }

};
