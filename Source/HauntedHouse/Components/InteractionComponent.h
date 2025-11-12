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

	TWeakObjectPtr<UInteractableComponent> InteractableComp;

	UPROPERTY(EditDefaultsOnly, Category = "InteractionSystem")
	float InteractionDist = 100.0f;
	UPROPERTY(EditDefaultsOnly, Category = "InteractionSystem")
	float InteractionRadius = 10.0f;
	// In terms of fps
	UPROPERTY(EditDefaultsOnly, Category = "InteractionSystem")
	float InteractionTickSpeed = 30.0f;
	float InteractionTickDeltaTime = 0.03f;

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
	
	void TryStartInteract();
	void CancelInteraction();
	
protected:

	void InteractionTick();

	bool CanInteract(const UInteractableComponent* interactable) const;
	
	UInteractableComponent* CheckForInteractable() const;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void HandleInteractionProgress(uint8 Progress);
	UFUNCTION()
	void HandleInteract();
	
	UFUNCTION(Server, Reliable)
	void Server_CancelInteraction(UInteractableComponent* interactableComponent) const;
	UFUNCTION(Server, Reliable)
	void Server_TryInteract(UInteractableComponent* Interactable);
	UFUNCTION(Client, Reliable)
	void Client_InteractionResponse(bool bInteractionSuccessful);
};
