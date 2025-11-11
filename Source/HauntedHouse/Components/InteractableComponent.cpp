// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableComponent.h"

#include "HauntedHouse/HauntedHouse.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
}

void UInteractableComponent::Server_TryToInteract_Implementation()
{
	if (!bInteractionActive)
	{
		bInteractionActive = true;
		UE_LOG(LogInteraction, Log, TEXT("Server_TryToInteract"));
		Client_StartInteract(true);
	}
	else
	{
		Client_StartInteract(false);
	}
}

void UInteractableComponent::Client_StartInteract_Implementation(bool bCanStartInteraction)
{
	UE_LOG(LogInteraction, Log, TEXT("Client_StartInteract_Implementation"));
	if (bCanStartInteraction)
	{
		StartInteraction();
	}
	else
	{
		OnUpdateInteractionProgressEvent.Clear();
	}
}

void UInteractableComponent::Server_CancelInteract_Implementation()
{
	bInteractionActive = false;
}

void UInteractableComponent::Server_InteractionComplete_Implementation()
{
	bHasBeenTriggered = true;
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

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

		if (OnUpdateInteractionProgressEvent.IsBound())
		{
			uint8 progress = static_cast<uint8>((CurrentHoldTime / HoldTime)*100);
			UE_LOG(LogInteraction, Log, TEXT("Progress: %d"), progress);
			OnUpdateInteractionProgressEvent.Execute(progress);
		}
	}
}

void UInteractableComponent::EndInteraction()
{
	if(OnInteractEvent.IsBound())
	{
		bHasBeenTriggered = true;	
		
		OnUpdateInteractionProgressEvent.Clear();
		OnInteractEvent.Broadcast();
	}
}

void UInteractableComponent::BeginHover() const
{
	if (OnBeginHoverEvent.IsBound())
	{
		OnBeginHoverEvent.Execute();
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
		OnEndHoverEvent.Execute();
	}
	else
	{
		UE_LOG(LogInteraction, Warning, TEXT("EndHover is not bound"));
	}
}

void UInteractableComponent::TryInteract()
{
	/*if (GetOwnerRole() != ROLE_Authority)
	{*/
		Server_TryToInteract();
	/*}
	else
	{
		Server_TryToInteract_Implementation();
	}*/
}

void UInteractableComponent::CancelInteraction()
{
	if(UWorld* world = GetWorld(); GetWorld() != nullptr && InteractionTimerHandle.IsValid())
	{
		world->GetTimerManager().ClearTimer(InteractionTimerHandle);
		InteractionTimerHandle.Invalidate();
	}
}

bool UInteractableComponent::CanInteract() const
{
	return (!bOnlyInteractOnce || (bOnlyInteractOnce && !bHasBeenTriggered)); 
}
