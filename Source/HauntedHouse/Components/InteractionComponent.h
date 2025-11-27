// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableComponent.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class UCameraComponent;
DECLARE_DYNAMIC_DELEGATE(FOnHUDBeginHover);
DECLARE_DYNAMIC_DELEGATE(FOnHUDEndHover);
DECLARE_DYNAMIC_DELEGATE(FOnHUDInteractionStart);
DECLARE_DYNAMIC_DELEGATE(FOnHUDInteractionEnd);
DECLARE_DYNAMIC_DELEGATE(FOnFailedToInteract);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnHUDInteractionProgress, uint8, Progress);

class UInteractableComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HAUNTEDHOUSE_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()
	
	// The camera component attached to the BaseCharacter class
	TWeakObjectPtr<UCameraComponent> CameraComp;
	FTimerHandle InteractionTimerHandle;

	UPROPERTY()
	TObjectPtr<UInteractableComponent> InteractableComp;

	// Distance in cm, how far the trace is for interacting
	UPROPERTY(EditDefaultsOnly, Category = "InteractionSystem")
	float InteractionDist = 200.0f;
	// Radius of sphere trace used for interacting
	UPROPERTY(EditDefaultsOnly, Category = "InteractionSystem")
	float InteractionRadius = 10.0f;
	// In terms of fps
	UPROPERTY(EditDefaultsOnly, Category = "InteractionSystem")
	float InteractionTickSpeed = 30.0f;
	// Translated delta time from InteractionTickSpeed variable. 
	float InteractionTickDeltaTime = 0.03f;

	// Flag  to keep track of whether the player is currently interacting or not
	bool bIsInteracting = false;

public:

	UPROPERTY()
	FOnHUDBeginHover OnHUDHoverBegin;
	UPROPERTY()
	FOnHUDEndHover OnHUDHoverEnd;
	UPROPERTY()
	FOnHUDInteractionStart OnHUDInteractionStart;
	UPROPERTY()
	FOnHUDInteractionEnd OnHUDInteractionEnd;
	UPROPERTY()
	FOnFailedToInteract OnFailedToInteract;
	UPROPERTY()
	FOnHUDInteractionProgress OnHUDInteractionProgress;
	
	// Sets default values for this component's properties
	UInteractionComponent();

	// Called by the base character, only on client
	void StartTimer();

	// Public function to start try starting interaction. 
	void TryStartInteract();
	// Public function to cancel interaction
	void CancelInteraction();
	
protected:

	// Tick event that handles raytracing and determining what the player is trying to interact with
	void InteractionTick();

	// Utility function that handles checks to see if player can interact with object.
	// This is necessary because object doesn't know the players net role
	bool CanInteract(const UInteractableComponent* interactable) const;

	// Raytracing logic used in InteractionTick
	UInteractableComponent* CheckForInteractable() const;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Handles response from interactable **/
	UFUNCTION()
	void HandleInteractionProgress(uint8 Progress);
	UFUNCTION()
	void HandleInteract();
	
	// Server RPC to try to interact with interactable. May fail because of conditionals or race conditions
	UFUNCTION(Server, Reliable)
	void Server_TryInteract(UInteractableComponent* Interactable);
	// Returns result of interaction attempt on server. If true client will bind to interaction event
	UFUNCTION(Client, Reliable)
	void Client_InteractionResponse(bool bInteractionSuccessful);
	// Server RPC to cancel interaction. Stops timer event
	UFUNCTION(Server, Reliable)
	void Server_CancelInteraction(UInteractableComponent* interactableComponent) const;
};
