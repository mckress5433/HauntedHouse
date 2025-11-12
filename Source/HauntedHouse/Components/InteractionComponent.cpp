// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "KismetTraceUtils.h"
#include "HauntedHouse/HauntedHouse.h"
#include "HauntedHouse/Character/InGameCharacter.h"
#include "HauntedHouse/Global/GlobalFunctionLibrary.h"


// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::TryStartInteract()
{
	Server_TryInteract(InteractableComp.Get());
}

void UInteractionComponent::Server_TryInteract_Implementation(UInteractableComponent* Interactable)
{
	if (Interactable != nullptr)
	{
		const bool interactionSuccessful = Interactable->TryToInteract();
		Client_InteractionResponse(interactionSuccessful);
	}
}

void UInteractionComponent::Client_InteractionResponse_Implementation(bool bInteractionSuccessful)
{
	if (GlobalFunctionLibrary::GetInteractionDebugValue() != 0)
	{
		UE_LOG(LogInteraction, Log, TEXT("CanStartInteraction: %s"), bInteractionSuccessful ? TEXT("true") : TEXT("false"));
	}

	if (bInteractionSuccessful)
	{
		// Start Interaction
		if (InteractableComp.IsValid())
		{
			if (!InteractableComp->OnUpdateInteractionProgressEvent.IsAlreadyBound(this, &UInteractionComponent::HandleInteractionProgress))
			{
				InteractableComp->OnUpdateInteractionProgressEvent.AddDynamic(this, &UInteractionComponent::HandleInteractionProgress);
			}
			if (!InteractableComp->OnInteractEvent.IsAlreadyBound(this, &UInteractionComponent::HandleInteract))
			{
				InteractableComp->OnInteractEvent.AddDynamic(this, &UInteractionComponent::HandleInteract);
			}
			InteractableComp->StartInteraction();
		}
		bIsInteracting = true;
		
		if (OnHUDInteractionStart.IsBound())
		{
			OnHUDInteractionStart.Execute();
		}
	}
	else
	{
		// Send failed to interaction event
		if (OnFailedToInteract.IsBound())
		{
			OnFailedToInteract.Execute();
		}
	}
}

void UInteractionComponent::CancelInteraction()
{
	if (GlobalFunctionLibrary::GetInteractionDebugValue() != 0)
	{
		UE_LOG(LogInteraction, Log, TEXT("CancelInteraction called"));
	}
	
	if (InteractableComp.IsValid())
	{
		bIsInteracting = false;
		if (InteractableComp->OnInteractEvent.IsAlreadyBound(this, &UInteractionComponent::HandleInteract))
		{
			InteractableComp->OnInteractEvent.RemoveDynamic(this, &UInteractionComponent::HandleInteract);
		}
		
		Server_CancelInteraction(InteractableComp.Get());
		
		if (OnHUDInteractionEnd.IsBound())
		{
			OnHUDInteractionEnd.Execute();
		}
	}
	else if (GlobalFunctionLibrary::GetInteractionDebugValue() != 0)
	{
		UE_LOG(LogInteraction, Log, TEXT("Cannot cancel because InteractableComponent is not valid!"));
	}
}


// Called when the game starts
void UInteractionComponent::StartTimer()
{
	if (auto character = static_cast<AInGameCharacter*>(GetOwner()); character != nullptr)
	{
		CameraComp = MakeWeakObjectPtr<UCameraComponent>(character->CameraComp);

		if (UWorld* world = character->GetWorld(); world != nullptr)
		{
			InteractionTickDeltaTime = 1/InteractionTickSpeed;
			world->GetTimerManager().SetTimer(InteractionTimerHandle, this, &UInteractionComponent::InteractionTick, InteractionTickDeltaTime, true);
		}
	}
}

void UInteractionComponent::InteractionTick()
{
	TWeakObjectPtr<UInteractableComponent> foundInteractable = MakeWeakObjectPtr<UInteractableComponent>(CheckForInteractable());
	
	if (foundInteractable.IsValid())
	{
		if (OnHUDHoverBegin.IsBound())
		{
			OnHUDHoverBegin.Execute();
		}

		bool bIsInteractableValid = InteractableComp.IsValid();
		if (!bIsInteractableValid || foundInteractable.Get() != InteractableComp.Get())
		{
			if (bIsInteractableValid)
			{
				InteractableComp->EndHover();
			}
			foundInteractable->BeginHover();
			InteractableComp = foundInteractable;
		}
	}
	else if (InteractableComp.IsValid())
	{
		if (bIsInteracting)
		{
			CancelInteraction();
		}
		
		InteractableComp->EndHover();
		InteractableComp.Reset();
		
		if (OnHUDHoverEnd.IsBound())
		{
			OnHUDHoverEnd.Execute();
		}
	}
}

bool UInteractionComponent::CanInteract(const UInteractableComponent* interactable) const
{
	return (interactable->CanInteract() || (bIsInteracting && interactable == InteractableComp.Get())) 
				&& (!interactable->GetIsHostOnly()
				|| (interactable->GetIsHostOnly() && GetOwnerRole() == ENetRole::ROLE_Authority));
}

UInteractableComponent* UInteractionComponent::CheckForInteractable() const
{
	UInteractableComponent* foundInteractable = nullptr;

	if (UWorld* world = GetWorld(); world != nullptr && CameraComp.IsValid())
	{
		if (UCameraComponent* cameraPtr = CameraComp.Get(false); cameraPtr != nullptr)
		{
			FVector cameraLocation = cameraPtr->GetComponentLocation();
			FVector CameraForwardVector = cameraPtr->GetForwardVector();
			FVector traceEnd = cameraLocation + (CameraForwardVector * InteractionDist);
			TArray<AActor*> IgnoredActors;
			IgnoredActors.Add(GetOwner());

			EDrawDebugTrace::Type debugFlag = static_cast<EDrawDebugTrace::Type>(GlobalFunctionLibrary::GetInteractionDebugValue());
			FCollisionQueryParams CollisionQueryParams;
			CollisionQueryParams.AddIgnoredActors(IgnoredActors);
			
			FHitResult lineTraceResult;
			
			world->LineTraceSingleByChannel(
				lineTraceResult,
				cameraLocation,
				traceEnd,
				ECC_Interactable,
				CollisionQueryParams
				);

			if (debugFlag != EDrawDebugTrace::None)
			{
				DrawDebugLineTraceSingle(
					world,
					cameraLocation,
					traceEnd,
					debugFlag,
					lineTraceResult.bBlockingHit,
					lineTraceResult,
					FColor::Green,
					FColor::Orange,
					InteractionTickDeltaTime);
			}

			// If we hit an interactable actor with the line trace then we are done here
			if (lineTraceResult.bBlockingHit)
			{
				if (AActor* hitActor = lineTraceResult.GetActor(); hitActor != nullptr)
				{
					if (auto interactableComp =	hitActor->FindComponentByClass<UInteractableComponent>(); interactableComp != nullptr)
					{
						if (CanInteract(interactableComp))
						{
							return interactableComp;
						}
					}
				}
			}

			// Otherwise we need consider the surrounding area
			FVector hitLocation = lineTraceResult.bBlockingHit ? lineTraceResult.ImpactPoint : traceEnd;
			float shortestDist = FMath::Sqrt(FMath::Square(InteractionRadius) + FMath::Square(InteractionDist));
			TArray<FHitResult> outHits;

			world->SweepMultiByChannel(
				outHits,
				cameraLocation,
				traceEnd,
				FQuat::Identity,
				ECC_Interactable,
				FCollisionShape::MakeSphere(InteractionRadius),
				CollisionQueryParams
				);

			bool bHit = false;
			
			for (FHitResult& hitResult : outHits)
			{
				if (hitResult.bBlockingHit)
				{
					bHit = true;
				}
				
				if (AActor* hitActor = hitResult.GetActor(); hitActor != nullptr)
				{
					if (auto interactableComp = hitActor->FindComponentByClass<UInteractableComponent>(); interactableComp != nullptr)
					{
						if (CanInteract(interactableComp))
						{
							float distToHit = (hitResult.ImpactPoint - hitLocation).Length();
							if (distToHit < shortestDist)
							{
								foundInteractable = interactableComp;
							}
						}
					}
				}
			}

			if (debugFlag != EDrawDebugTrace::None)
			{
				DrawDebugSphereTraceMulti(
					world,
					cameraLocation,
					traceEnd,
					InteractionRadius,
					debugFlag,
					bHit,
					outHits,
					FColor::Cyan,
					FColor::Red,
					InteractionTickDeltaTime);
			}
		}
	}
	return foundInteractable;
}

void UInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	InteractionTimerHandle.Invalidate();
}

void UInteractionComponent::HandleInteractionProgress(uint8 Progress)
{
	OnHUDInteractionProgress.Execute(Progress);
}

void UInteractionComponent::HandleInteract()
{
	bIsInteracting = false;
	if (InteractableComp.IsValid() && InteractableComp->OnInteractEvent.IsAlreadyBound(this, & UInteractionComponent::HandleInteract))
	{
		InteractableComp->OnInteractEvent.RemoveDynamic(this, &UInteractionComponent::HandleInteract);
	}
}


void UInteractionComponent::Server_CancelInteraction_Implementation(UInteractableComponent* interactableComponent) const
{
	if (interactableComponent != nullptr)
	{
		interactableComponent->CancelInteraction();
	}
}




