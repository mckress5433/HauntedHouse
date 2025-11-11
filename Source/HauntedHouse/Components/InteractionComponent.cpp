// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"

#include "KismetTraceUtils.h"
#include "HauntedHouse/HauntedHouse.h"
#include "HauntedHouse/Character/InGameCharacter.h"
#include "HauntedHouse/Global/GlobalConsoleVariables.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::TryStartInteract() const
{
	if (InteractableComp != nullptr)
	{
		InteractableComp->TryInteract();
		InteractableComp->OnUpdateInteractionProgressEvent.BindDynamic(this, &UInteractionComponent::HandleInteractionProgress);
		OnHUDInteractionStart.Execute();
	}
}

void UInteractionComponent::CancelInteraction() const
{
	if (InteractableComp != nullptr)
	{
		InteractableComp->CancelInteraction();
		OnHUDInteractionEnd.Execute();
	}
}


// Called when the game starts
void UInteractionComponent::StartTimer()
{
	if (auto character = static_cast<AInGameCharacter*>(GetOwner()); character != nullptr)
	{
		CameraComp = MakeWeakObjectPtr<UCameraComponent>(character->CameraComp);
		//PlayerHud = MakeWeakObjectPtr<UPlayerHud>(character->GetPlayerHud());

		if (UWorld* world = character->GetWorld(); world != nullptr)
		{
			InteractionTickDeltaTime = 1/InteractionTickSpeed;
			world->GetTimerManager().SetTimer(InteractionTimerHandle, this, &UInteractionComponent::InteractionTick, InteractionTickDeltaTime, true);
		}
	}
}

void UInteractionComponent::InteractionTick()
{
	TWeakObjectPtr<UInteractableComponent> foundInteractable = CheckForInteractable();

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
		InteractableComp->EndHover();
		InteractableComp.Reset();

		
		if (OnHUDHoverEnd.IsBound())
		{
			OnHUDHoverEnd.Execute();
		}
	}
	
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

			EDrawDebugTrace::Type debugFlag = static_cast<EDrawDebugTrace::Type>(GInteractionDebugVariable.GetValueOnAnyThread(false));
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
						if (interactableComp->CanInteract())
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
						if (interactableComp->CanInteract())
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




