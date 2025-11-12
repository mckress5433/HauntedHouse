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

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_CurrentHoldTime)
	float CurrentHoldTime;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_HasBeenTriggered)
	bool bHasBeenTriggered = false;

	// Whether this component is being interacted.
	// Used to prevent multiple players from interacting at the same time
	UPROPERTY(Replicated)
	bool bInteractionActive = false;

	
protected:
	UPROPERTY(EditAnywhere)
	bool bHostOnly = false;
	// The amount of time the player must hold the interaction button to complete the interaction
	UPROPERTY(EditAnywhere)
	float HoldTime = 0.0f;
	// The tick rate in seconds for the interaction tick
	UPROPERTY(EditAnywhere)
	float InteractionDeltaTime = 0.03f;

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
	
	void InteractionTick();
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
	void StartInteraction();
	void CancelInteraction();

	bool CanInteract() const;
	bool GetIsHostOnly() const { return bHostOnly; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetHasBeenTriggered() { return bHasBeenTriggered; }
};
