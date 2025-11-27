// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableComponent.generated.h"

class UInteractionComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginHover);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndHover);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractCanceled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateInteractionProgress, uint8, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteract);


UCLASS(Blueprintable, ClassGroup="HauntedHouse", hidecategories=(Object,Activation,"Components|Activation",Sockets,Base,Lighting,LOD,Mesh), editinlinenew, meta=(BlueprintSpawnableComponent))
class HAUNTEDHOUSE_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()
	UInteractableComponent();
	
	FTimerHandle InteractionTimerHandle;

	// How long the interaction has been held for so far
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHoldTime)
	float CurrentHoldTime = 0.0f;

	// A flag that represents whether interaction is complete or not
	UPROPERTY(ReplicatedUsing = OnRep_HasBeenTriggered)
	bool bHasBeenTriggered = false;

	// Whether this component is being interacted.
	// Used to prevent multiple players from interacting at the same time
	UPROPERTY(Replicated)
	bool bInteractionActive = false;

	
protected:
	// Whether the host is the only player allowed to interact with the object or not
	UPROPERTY(EditAnywhere)
	bool bHostOnly = false;
	// The amount of time the player must hold the interaction button to complete the interaction
	UPROPERTY(EditAnywhere)
	float HoldTime = 0.0f;
	// The tick rate in seconds for the interaction tick
	UPROPERTY(EditAnywhere)
	float InteractionDeltaTime = 0.03f;
	// Can the object be interacted with multiple times
	UPROPERTY(EditAnywhere)
	bool bOnlyInteractOnce = false;
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnBeginHover OnBeginHoverEvent;
	UPROPERTY(BlueprintAssignable)
	FOnEndHover OnEndHoverEvent;
	UPROPERTY(BlueprintAssignable)
	FOnInteract OnInteractEvent;
	UPROPERTY(BlueprintAssignable)
	FOnUpdateInteractionProgress OnUpdateInteractionProgressEvent;
	UPROPERTY(BlueprintAssignable)
	FOnInteractStart OnInteractStartEvent;
	UPROPERTY(BlueprintAssignable)
	FOnInteractCanceled OnInteractCanceledEvent;
	
	
	/*
	 * Functions
	 */
private:

	
protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// Interaction tick event. Only runs on server. Updates the CurrentHoldTime
	void InteractionTick();
	// Triggered when the interaction is complete
	void EndInteraction();

	UFUNCTION()
	void OnRep_HasBeenTriggered();
	UFUNCTION()
	void OnRep_CurrentHoldTime();
	
public:
	
	// Triggered when InteractionComponent::InteractableComp is set to this object
	void BeginHover() const;
	// Triggered when InteractionComponent::InteractableComp is no longer set to this object
	void EndHover() const;

	// Ask server if InteractionComponent can interact.
	// Return true if it is able to interact
	bool TryToInteract();
	// Start timer event that calls "InteractionTick" function
	void StartInteraction();
	// Cancels "InteractionTick" event
	void CancelInteraction();

	// Returns whether the interactable can be interacted with
	bool CanInteract() const;
	bool GetIsHostOnly() const { return bHostOnly; }

	/** Getter functions for owning actors **/
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetHasBeenTriggered() { return bHasBeenTriggered; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetOnlyInteractOnce() { return bOnlyInteractOnce; }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentHoldTime() { return CurrentHoldTime; }
};
