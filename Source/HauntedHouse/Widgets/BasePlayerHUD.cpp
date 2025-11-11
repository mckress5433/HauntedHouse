// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerHUD.h"

#include "HauntedHouse/Character/InGameCharacter.h"

void UBasePlayerHUD::HideInteractionPrompt_Implementation()
{
	// Implemented in blueprints
}

void UBasePlayerHUD::ShowInteractionPrompt_Implementation()
{
	// Implemented in blueprints
}

void UBasePlayerHUD::UpdateInteractionProgress_Implementation(uint8 ProgressPercentage)
{
	// Implemented in blueprints
}

void UBasePlayerHUD::StartInteraction_Implementation()
{
	// Implemented in blueprints
}

void UBasePlayerHUD::EndInteraction_Implementation()
{
	// Implemented in blueprints
}

void UBasePlayerHUD::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (auto Character = Cast<AInGameCharacter>(GetOwningPlayerPawn()); Character != nullptr)
	{
		if (UInteractionComponent* interactionComponent = Character->GetInteractionComponent(); interactionComponent != nullptr)
		{
			interactionComponent->OnHUDHoverBegin.BindDynamic(this, &UBasePlayerHUD::ShowInteractionPrompt);
			interactionComponent->OnHUDHoverEnd.BindDynamic(this, &UBasePlayerHUD::HideInteractionPrompt);
			interactionComponent->OnHUDInteractionStart.BindDynamic(this, &UBasePlayerHUD::StartInteraction);
			interactionComponent->OnHUDInteractionEnd.BindDynamic(this, &UBasePlayerHUD::EndInteraction);
			interactionComponent->OnHUDInteractionProgress.BindDynamic(this, &UBasePlayerHUD::UpdateInteractionProgress);
		}
	}
}


