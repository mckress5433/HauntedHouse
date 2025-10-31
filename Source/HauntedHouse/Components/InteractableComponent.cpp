// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableComponent.h"

#include "HauntedHouse/Widgets/InteractionWidget.h"

// Sets default values for this component's properties
UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	SetHiddenInGame(true);
}

void UInteractableComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	CancelInteraction();
}

void UInteractableComponent::InteractionTick()
{
	if(CurrentHoldTime >= HoldTime)
	{
		EndInteraction();
		InteractionTimerHandle.Invalidate();
	}
	else
	{
		CurrentHoldTime += InteractionDeltaTIme;
		if(const auto InteractionWidget = Cast<UInteractionWidget>(GetWidget()); InteractionWidget != nullptr)
		{
			InteractionWidget->UpdateInteractionProgress(CurrentHoldTime/HoldTime);
		}
	}
}

void UInteractableComponent::EndInteraction()
{
	if(OnInteract.IsBound())
	{
		if(bOnlyInteractOnce)
		{
			bHasBeenTriggered = true;	
		}
		
		OnInteract.Broadcast();
	}
}

void UInteractableComponent::Interact()
{
	if(HoldTime <= 0.0f)
	{
		EndInteraction();
	}
	else
	{
		if(const UWorld* world = GetWorld(); world != nullptr)
		{
			world->GetTimerManager().SetTimer(InteractionTimerHandle, this, &ThisClass::InteractionTick, InteractionDeltaTIme, true);
		}
	}
}

void UInteractableComponent::CancelInteraction()
{
	if(InteractionTimerHandle.IsValid() && GetWorld() != nullptr)
	{
		GetWorld()->GetTimerManager().ClearTimer(InteractionTimerHandle);
		InteractionTimerHandle.Invalidate();
		if(const auto InteractionWidget = Cast<UInteractionWidget>(GetWidget()); InteractionWidget != nullptr)
		{
			CurrentHoldTime = 0.0f;
			InteractionWidget->UpdateInteractionProgress(CurrentHoldTime/HoldTime);
		}
	}
}

void UInteractableComponent::ShowInteractionText()
{
	bIsActive = true;
	SetHiddenInGame(false);
	
}

void UInteractableComponent::HideInteractionText()
{
	bIsActive = false;
	SetHiddenInGame(true);
	CancelInteraction();
}
