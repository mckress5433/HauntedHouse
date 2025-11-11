// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InteractableComponent.generated.h"

class UInteractionComponent;
DECLARE_DYNAMIC_DELEGATE(FOnBeginHover);
DECLARE_DYNAMIC_DELEGATE(FOnEndHover);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUpdateInteractionProgress, uint8, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteract);

UCLASS(Blueprintable, ClassGroup="HauntedHouse", hidecategories=(Object,Activation,"Components|Activation",Sockets,Base,Lighting,LOD,Mesh), editinlinenew, meta=(BlueprintSpawnableComponent))
class HAUNTEDHOUSE_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()
	UInteractableComponent();
	
	FTimerHandle InteractionTimerHandle;
	float CurrentHoldTime;

	UPROPERTY(Replicated)
	float bHasBeenTriggered = false;

	// Whether this component is being interacted.
	// Used to prevent multiple players from interacting at the same time
	UPROPERTY(Replicated)
	bool bInteractionActive = false;

	
protected:
	UPROPERTY(EditDefaultsOnly)
	bool bHostOnly = false;
	// The amount of time the player must hold the interaction button to complete the interaction
	UPROPERTY(EditDefaultsOnly)
	float HoldTime = 0.0f;
	// The tick rate in seconds for the interaction tick
	UPROPERTY(EditDefaultsOnly)
	float InteractionDeltaTime = 0.03f;

	UPROPERTY(EditDefaultsOnly)
	bool bOnlyInteractOnce = false;
	
public:
	UPROPERTY()
	FOnBeginHover OnBeginHoverEvent;
	UPROPERTY()
	FOnEndHover OnEndHoverEvent;
	UPROPERTY()
	FOnInteract OnInteractEvent;
	UPROPERTY()
	FOnUpdateInteractionProgress OnUpdateInteractionProgressEvent;
	
	/*
	 * Functions
	 */
private:

	// Ask server if client can interact.
	// If client can interact with component then Client_StartInteraction is called
	UFUNCTION(Server, Reliable)
	void Server_TryToInteract();

	// bCanStartInteraction so that binding to UpdateInteractionProgressEvent can be cleared
	UFUNCTION(Client, Reliable)
	void Client_StartInteract(bool bCanStartInteraction);

	UFUNCTION(Server, Reliable)
	void Server_CancelInteract();
	UFUNCTION(Server, Reliable)
	void Server_InteractionComplete();
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void StartInteraction();
	void InteractionTick();
	void EndInteraction();
	
public:
	
	// Triggered when InteractionComponent::InteractableComp is set to this object
	void BeginHover() const;
	// Triggered when InteractionComponent::InteractableComp is no longer set to this object
	void EndHover() const;

	// Called from the InteractionComponent
	void TryInteract();
	void CancelInteraction();

	bool CanInteract() const;
	bool GetIsHostOnly() const { return bHostOnly; }

};
