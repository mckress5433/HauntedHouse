// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameCharacter.h"

#include "KismetTraceUtils.h"
#include "Engine/SkinnedAssetCommon.h"
#include "GeometryCollection/Facades/CollectionConstraintOverrideFacade.h"
#include "HauntedHouse/Components/InteractableComponent.h"
#include "HauntedHouse/Player/InGamePlayerController.h"
#include "HauntedHouse/Player/PlayerState/InGamePlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Sections/MovieScene3DTransformSection.h"

DEFINE_LOG_CATEGORY(LogCharacter)

int32 AInGameCharacter::bDrawDebug = 0;
FAutoConsoleVariableRef AInGameCharacter::CVarDrawDebug(
	TEXT("Interaction.DebugInteractionTrace"),				// CVar name
	AInGameCharacter::bDrawDebug,						// Variable to modify
	TEXT("Enable or disable debug drawing for sphere trace (0 = Disable, 1 = Enable)"), // Help text
	ECVF_Default										// Flags (Default means it's modifiable at runtime)
);

AInGameCharacter::AInGameCharacter()
{
	bReplicates = true;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(FName("CameraComp"));
	CameraComp->SetupAttachment(GetMesh());

	WidgetInteractionComp = CreateDefaultSubobject<UWidgetInteractionComponent>(FName("WidgetInteractionComp"));
	WidgetInteractionComp->SetupAttachment(CameraComp);
}

void AInGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(GetLocalRole() == ROLE_Authority && IsLocallyControlled())
	{
		if (auto PS = GetPlayerState<AInGamePlayerState>(); PS != nullptr)
		{
			if(UBaseCharacterDataAsset* characterDA = PS->GetBaseCharacterDataAsset(); characterDA != nullptr)
			{
				UpdateMeshes(characterDA->GetCharacterMeshData(), characterDA->GetCharacterColor());	
			}
		}
	}
		
	if(UWorld* world = GetWorld(); world != nullptr && IsLocallyControlled())
	{
		world->GetTimerManager().SetTimer(InteractionZoneTickHandle, this, &ThisClass::InteractionZoneTick, InteractionTickRate, true);		
	}
}

void AInGameCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (auto PS = GetPlayerState<AInGamePlayerState>(); PS != nullptr)
	{
		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		if(AbilitySystemComponent = Cast<UCharacterAbilitySystemComponent>(PS->GetAbilitySystemComponent());
			AbilitySystemComponent != nullptr)
		{
			// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
			AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		}
		else
		{
			UE_LOG(LogCharacter, Error, TEXT("Could not set AbilitySystemComponent!"));	
		}
	}
}

void AInGameCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (auto PS = GetPlayerState<AInGamePlayerState>(); PS != nullptr)
	{
		// Set the ASC for clients. Server does this in PossessedBy.
		if(AbilitySystemComponent = Cast<UCharacterAbilitySystemComponent>(PS->GetAbilitySystemComponent());
			AbilitySystemComponent != nullptr)
		{
			// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
			AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		}
		else
		{
			UE_LOG(LogCharacter, Warning, TEXT("Failed to update ability ability system component. PlayerState return nullptr"));
		}

		if(UBaseCharacterDataAsset* characterDA = PS->GetBaseCharacterDataAsset(); characterDA != nullptr)
		{
			UpdateMeshes(characterDA->GetCharacterMeshData(), characterDA->GetCharacterColor());	
		}
		UpdateMeshes_Multicast();
	}
}

void AInGameCharacter::InteractionZoneTick()
{
	UWorld* world = GetWorld();
	if(world == nullptr) return;
	
	AActor* InteractableOfInterest = nullptr;
	TArray<AActor*> InteractableActors;

	// Start and end locations for the trace
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + (GetActorForwardVector() * TraceDistance);

	// Parameters for the trace
	FCollisionQueryParams TraceParams(FName(TEXT("SphereTrace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;

	// Struct to store trace results
	TArray<FHitResult> HitResults;

	// Perform the sphere trace
	bool bHit = world->SweepMultiByChannel(
		HitResults,                  // The result of the trace (what it hit)
		StartLocation,              // Starting point of the trace
		EndLocation,                // End point of the trace
		FQuat::Identity,            // Rotation (none in this case)
		TraceChannel,               // Trace channel (e.g., ECC_Visibility, ECC_PhysicsBody)
		FCollisionShape::MakeSphere(SphereRadius),  // Sphere radius
		TraceParams                 // Additional trace parameters
	);

	if(bDrawDebug)
	{
		DrawDebugSphereTraceMulti(
			world,
			StartLocation,
			EndLocation,
			SphereRadius,
			EDrawDebugTrace::ForDuration,
			bHit, HitResults,
			FLinearColor::Green,
			FLinearColor::Red,
			InteractionTickRate);
	}
	
	for(FHitResult hitResult : HitResults)
	{
		if(AActor* overlappedActor = hitResult.GetActor(); overlappedActor != nullptr)
		{
			if(const auto InteractableComponent =  overlappedActor->FindComponentByClass<UInteractableComponent>();
			InteractableComponent != nullptr && InteractableComponent->CanInteract())
			{
				if(InteractableComponent->GetIsHostOnly())
				{
					if(GetLocalRole() == ROLE_Authority)
					{
						InteractableActors.Add(overlappedActor);
					}
				}
				else
				{
					InteractableActors.Add(overlappedActor);
				}
			}
		}
	}

	if(InteractableActors.Num() > 0)
	{
		if(InteractableActors.Num() == 1)
		{
			InteractableOfInterest = InteractableActors[0];
		}
		else
		{
			if(CameraComp != nullptr)
			{
				// Struct to store trace results
				FHitResult HitResult;
				
				// Perform the sphere trace
				bHit = GetWorld()->LineTraceSingleByChannel(
					HitResult,                  // The result of the trace (what it hit)
					StartLocation,              // Starting point of the trace
					EndLocation,                // End point of the trace
					TraceChannel,               // Trace channel (e.g., ECC_Visibility, ECC_PhysicsBody)
					TraceParams                 // Additional trace parameters
				);

				// Check if something was hit
				if (bHit)
				{
					if(bDrawDebug == 1)
					{
						// Optionally draw a debug sphere at the hit location
						DrawDebugLine(GetWorld(), StartLocation, HitResult.ImpactPoint, FColor::Red, false, 5.0f);
					}

					if(AActor* HitActor = HitResult.GetActor(); HitActor != nullptr)
					{
						if(auto InteractableComponent = HitActor->FindComponentByClass<UInteractableComponent>(); InteractableComponent != nullptr && InteractableComponent->CanInteract())
						{
							InteractableOfInterest = HitActor;
						}
					}
					else
					{
						AActor* closestActor = nullptr;
						float closestDist = TraceDistance;
						for(AActor* actor : InteractableActors)
						{
							float distToImpact = (actor->GetActorLocation() - HitResult.ImpactPoint).Length();
							if(distToImpact < closestDist)
							{
								closestActor = actor;
							}
						}

						if(closestActor != nullptr)
						{
							InteractableOfInterest = closestActor;
						}
					}
				}
				else
				{
					if(bDrawDebug == 1)
					{
						// No hit, draw the path of the sphere trace
						DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 5.0f, 0, 1.0f);
					}
				}
			}
		}
	}

	if(FocusedInteractable != InteractableOfInterest)
	{
		if(FocusedInteractable != nullptr)
		{
			if(UInteractableComponent* interactableComponent = FocusedInteractable->FindComponentByClass<UInteractableComponent>(); interactableComponent != nullptr)
			{
				interactableComponent->HideInteractionText();
				FocusedInteractable = nullptr;
			}
		}
		
		if(InteractableOfInterest != nullptr)
		{
			InteractableOfInterest->FindComponentByClass<UInteractableComponent>()->ShowInteractionText();
			FocusedInteractable = InteractableOfInterest;
		}
	}
}

void AInGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AInGameCharacter::UpdateCameraRotationInterpolation()
{
	if (CameraComp == nullptr || FocusTarget == nullptr) return;

	// Update elapsed time
	ElapsedTimeDuringInterpolation += 0.01f;

	// Compute the progress of interpolation (clamped between 0 and 1)
	float Alpha = FMath::Clamp(ElapsedTimeDuringInterpolation / CameraInterpolationDuration, 0.0f, 1.0f);

	// Interpolate between the initial and target rotations
	FRotator NewRotation = FMath::InterpEaseIn(InitialCameraRotation, GetTargetCameraRotation(), Alpha, 2.0f);

	// Apply the rotation to the camera component
	CameraComp->SetWorldRotation(NewRotation);

	// Stop interpolation if it is complete
	if (Alpha >= 1.0f)
	{
		CancelCameraInterpolation();
	}

}

FRotator AInGameCharacter::GetTargetCameraRotation()
{
	if (CameraComp == nullptr || FocusTarget == nullptr) return FRotator::ZeroRotator;
	
	// Calculate the target rotation from the camera's position to the target position
	FVector CameraLocation = CameraComp->GetComponentLocation();
	FVector DirectionToTarget = (FocusTarget->GetActorLocation() - CameraLocation).GetSafeNormal();
	return FRotationMatrix::MakeFromX(DirectionToTarget).Rotator();
}

void AInGameCharacter::HandleMoveInput(const FVector2D& InputVector)
{
	// Get the forward and right vectors based on camera orientation
	const FVector ForwardVector = GetActorForwardVector();
	const FVector RightVector = GetActorRightVector();

	// Apply movement in both directions
	AddMovementInput(ForwardVector, InputVector.Y);
	AddMovementInput(RightVector, InputVector.X);
}

void AInGameCharacter::HandleInteractionInput_Start()
{
	if(FocusedInteractable != nullptr)
	{
		if(auto InteractableComponent = FocusedInteractable->FindComponentByClass<UInteractableComponent>(); InteractableComponent != nullptr)
		{
			InteractableComponent->Interact();
		}
	}
}

void AInGameCharacter::HandleInteractionInput_End()
{
	if(FocusedInteractable != nullptr)
	{
		if(auto InteractableComponent = FocusedInteractable->FindComponentByClass<UInteractableComponent>(); InteractableComponent != nullptr)
		{
			InteractableComponent->CancelInteraction();
		}
	}
}

void AInGameCharacter::HandleUIInteractionInput()
{
	if (WidgetInteractionComp !=  nullptr && WidgetInteractionComp->IsActive())
	{
		WidgetInteractionComp->PressPointerKey(FKey(EKeys::LeftMouseButton));
		WidgetInteractionComp->ReleasePointerKey(FKey(EKeys::LeftMouseButton));
	}
}

UAbilitySystemComponent* AInGameCharacter::GetAbilitySystemComponent() const
{
	if(AbilitySystemComponent != nullptr)
	{
		return AbilitySystemComponent;
	}
	else
	{
		if(auto PS = GetPlayerState<AInGamePlayerState>(); PS != nullptr)
		{
			return Cast<UCharacterAbilitySystemComponent>(PS->GetAbilitySystemComponent());
		}
		else
		{
			UE_LOG(LogCharacter, Warning, TEXT("Could not get ability system component because player state is null"));
		}
	}
	
	return nullptr;
}

void AInGameCharacter::UpdateMeshes(FCharacterMeshData CharacterMeshData, FColor MeshColor)
{
	USkeletalMesh* FPSkelMesh = CharacterMeshData.FirstPersonMesh;
	USkeletalMesh* TPSkelMesh = CharacterMeshData.ThirdPersonMesh;

	if(FPSkelMesh == nullptr || TPSkelMesh == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("First Person or ThirdPerson mesh is invalid"));
		return;
	}
	
	if(IsLocallyControlled())
	{
		SetFirstPersonMesh(FPSkelMesh);
	}
	else
	{
		SetThirdPersonMesh(TPSkelMesh);
	}

	if(USkeletalMeshComponent* CharMeshComp = GetMesh(); CharMeshComp != nullptr)
	{
		if(DynamicMaterialInstances.Num() != 0)
		{
			DynamicMaterialInstances.Empty();
			for(int i=0; i<FPSkelMesh->GetMaterials().Num(); i++)
			{
				UMaterialInterface* materialInterface = FPSkelMesh->GetMaterials()[i].MaterialInterface;
				UMaterialInstanceDynamic* dynamicMaterialInstance = UMaterialInstanceDynamic::Create(materialInterface, this);
				DynamicMaterialInstances.Add(dynamicMaterialInstance);
				
				if(dynamicMaterialInstance != nullptr)
				{
					// Apply the dynamic material to the mesh
					CharMeshComp->SetMaterial(i, dynamicMaterialInstance);
					dynamicMaterialInstance->SetVectorParameterValue(TEXT("Tint"), MeshColor);
				}
			}
		}
		else
		{
			for(int i = 0; i<CharMeshComp->GetMaterials().Num(); i++)
			{
				UMaterialInterface* materialInterface = CharMeshComp->GetMaterial(i);
				UMaterialInstanceDynamic* dynamicMaterialInstance = UMaterialInstanceDynamic::Create(materialInterface, this);
				DynamicMaterialInstances.Add(dynamicMaterialInstance);
				
				if(dynamicMaterialInstance != nullptr)
				{
					// Apply the dynamic material to the mesh
					CharMeshComp->SetMaterial(i, dynamicMaterialInstance);
					dynamicMaterialInstance->SetVectorParameterValue(TEXT("Tint"), MeshColor);
				}
			}
		}
	}
}

void AInGameCharacter::ToggleUsePawnControlRotation(bool newState)
{
	CameraComp->bUsePawnControlRotation = newState;
}

void AInGameCharacter::StartCameraRotationInterpolation(AActor* LookAtTarget, float InterpolationDuration)
{
	if (CameraComp == nullptr || LookAtTarget == nullptr) return;

	FocusTarget = LookAtTarget;

	InitialCameraRotation = CameraComp->GetComponentRotation();
	// Set the interpolation duration and reset elapsed time
	CameraInterpolationDuration = InterpolationDuration;
	ElapsedTimeDuringInterpolation = 0.0f;
	
	GetWorld()->GetTimerManager().SetTimer(
		CameraInterpolationTimerHandle,
		this,
		&AInGameCharacter::UpdateCameraRotationInterpolation,
		0.01f,   // Tick every 0.01 seconds
		true
	);
}

void AInGameCharacter::CancelCameraInterpolation()
{
	if (CameraComp == nullptr) return;

	// Stop the timer
	GetWorld()->GetTimerManager().ClearTimer(CameraInterpolationTimerHandle);
	CameraInterpolationTimerHandle.Invalidate();
	
	Controller->SetControlRotation(GetTargetCameraRotation());
}

void AInGameCharacter::ToggleWidgetInteractionActivation(bool bIsActive)
{
	WidgetInteractionComp->SetActive(bIsActive);
}

void AInGameCharacter::SetThirdPersonMesh(USkeletalMesh* NewMesh)
{
	USkeletalMeshComponent* CharMeshComp = GetMesh();
	if(CharMeshComp == nullptr || IsLocallyControlled())
	{
		return;
	}

	ThirdPersonMesh = NewMesh;
	CharMeshComp->SetSkeletalMesh(ThirdPersonMesh);

	if(ThirdPersonAnimBP != nullptr)
	{
		CharMeshComp->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		CharMeshComp->SetAnimInstanceClass(ThirdPersonAnimBP);
	}
}

void AInGameCharacter::UpdateMeshes_Multicast_Implementation()
{
	if (auto PS = GetPlayerState<AInGamePlayerState>(); PS != nullptr)
	{
		if(UBaseCharacterDataAsset* characterDA = PS->GetBaseCharacterDataAsset(); characterDA != nullptr)
		{
			UpdateMeshes(characterDA->GetCharacterMeshData(), characterDA->GetCharacterColor());	
		}
	}
}

void AInGameCharacter::SetFirstPersonMesh(USkeletalMesh* NewMesh)
{
	USkeletalMeshComponent* CharMeshComp = GetMesh();
	if(CharMeshComp == nullptr)
	{
		return;
	}
	
	FirstPersonMesh = NewMesh;
	CharMeshComp->SetSkeletalMesh(FirstPersonMesh);
}

