// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableComponent.h"

#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "HauntedHouse/HauntedHouse.h"
#include "HauntedHouse/Global/GlobalFunctionLibrary.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractableComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	CancelInteraction();
}

void UInteractableComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, bHasBeenTriggered);
	DOREPLIFETIME(ThisClass, bInteractionActive);
}

void UInteractableComponent::StartInteraction()
{
	if(HoldTime <= 0.0f)
	{
		EndInteraction();
	}
	else
	{
		if(const UWorld* world = GetWorld(); world != nullptr)
		{
			world->GetTimerManager().SetTimer(InteractionTimerHandle, this, &ThisClass::InteractionTick, InteractionDeltaTime, true);
		}

		if (OnInteractStartEvent.IsBound())
		{
			OnInteractStartEvent.Broadcast();
		}
	}
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
		CurrentHoldTime += InteractionDeltaTime;
		OnRep_CurrentHoldTime();
		if (GlobalFunctionLibrary::GetInteractionDebugValue() != 0)
		{
			uint8 progress = static_cast<uint8>((CurrentHoldTime / HoldTime)*100);
			UE_LOG(LogInteraction, Log, TEXT("Progress: %d"), progress);
		}
	}
}

void UInteractableComponent::EndInteraction()
{
	if(UWorld* world = GetWorld(); GetWorld() != nullptr && InteractionTimerHandle.IsValid())
	{
		world->GetTimerManager().ClearTimer(InteractionTimerHandle);
		InteractionTimerHandle.Invalidate();
	}

	CurrentHoldTime = 0.0f;
	bHasBeenTriggered = true;
	OnRep_HasBeenTriggered();
	bInteractionActive = false;
}

void UInteractableComponent::OnRep_HasBeenTriggered()
{
	if (OnInteractEvent.IsBound())
	{
		OnInteractEvent.Broadcast();
	}
}

void UInteractableComponent::OnRep_CurrentHoldTime()
{
	if (OnUpdateInteractionProgressEvent.IsBound())
	{
		uint8 progress = static_cast<uint8>((CurrentHoldTime / HoldTime)*100);
		if (GlobalFunctionLibrary::GetInteractionDebugValue() != 0)
		{
			GEngine->AddOnScreenDebugMessage(1, InteractionDeltaTime, FColor::Magenta, FString::Printf(TEXT("OnRep_CurrentHoldTime: %d"), progress));	
		}
		OnUpdateInteractionProgressEvent.Broadcast(progress);
	}
}

void UInteractableComponent::BeginHover() const
{
	if (OnBeginHoverEvent.IsBound())
	{
		OnBeginHoverEvent.Broadcast();
	}
	else
	{
		UE_LOG(LogInteraction, Warning, TEXT("BeginHover is not bound"));
	}
}

void UInteractableComponent::EndHover() const
{
	if (OnEndHoverEvent.IsBound())
	{
		OnEndHoverEvent.Broadcast();
	}
	else
	{
		UE_LOG(LogInteraction, Warning, TEXT("EndHover is not bound"));
	}
}

bool UInteractableComponent::TryToInteract()
{
	// Double check before interacting. Server may have a different value due to delay
	if (CanInteract())
	{
		StartInteraction();
		bInteractionActive = true;
		return true;
	}
	return false;
}

void UInteractableComponent::CancelInteraction()
{
	if(UWorld* world = GetWorld(); GetWorld() != nullptr && InteractionTimerHandle.IsValid())
	{
		world->GetTimerManager().ClearTimer(InteractionTimerHandle);
		InteractionTimerHandle.Invalidate();
	}

	CurrentHoldTime = 0.0f;
	if (!bOnlyInteractOnce)
	{
		bHasBeenTriggered = false;
		OnRep_HasBeenTriggered();
	}

	if (OnInteractCanceledEvent.IsBound())
	{
		OnInteractCanceledEvent.Broadcast();
	}

	bInteractionActive = false;
}

bool UInteractableComponent::CanInteract() const
{
	return (!bOnlyInteractOnce || (bOnlyInteractOnce && !bHasBeenTriggered)) && !bInteractionActive; 
}
