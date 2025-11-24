// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameCharacter.h"
#include "Engine/SkinnedAssetCommon.h"
#include "HauntedHouse/HauntedHouse.h"
#include "HauntedHouse/Player/PlayerState/InGamePlayerState.h"
#include "Net/UnrealNetwork.h"

AInGameCharacter::AInGameCharacter()
{
	bReplicates = true;

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(FName("InteractionComponent"));
}

void AInGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(GetLocalRole() == ROLE_Authority && IsLocallyControlled())
	{
		if (auto PS = GetPlayerState<AInGamePlayerState>(); PS != nullptr)
		{
			FPlayersCharacterInfo CharacterInfo = PS->GetCharacterInfo();
			UpdateMeshes(CharacterInfo.CharacterMeshData, CharacterInfo.CharacterColor);
		}
	}
		
	if (InteractionComponent != nullptr && Controller != nullptr && Controller->IsLocalController())
	{
		InteractionComponent->StartTimer();
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

		FPlayersCharacterInfo CharacterInfo = PS->GetCharacterInfo();
		UpdateMeshes(CharacterInfo.CharacterMeshData, CharacterInfo.CharacterColor);
		if (GetLocalRole() == ROLE_Authority)
		{
			UpdateMeshes_Multicast(CharacterInfo);
		}
	}
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

void AInGameCharacter::HandleInteractionInput_Start() const
{
	if (InteractionComponent != nullptr)
	{
		InteractionComponent->TryStartInteract();
	}
}

void AInGameCharacter::HandleInteractionInput_End() const
{
	if (InteractionComponent != nullptr)
	{
		InteractionComponent->CancelInteraction();
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

void AInGameCharacter::UpdateMeshes(const FCharacterMeshData& CharacterMeshData, const FColor& MeshColor)
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

void AInGameCharacter::ToggleUsePawnControlRotation(bool newState) const
{
	CameraComp->bUsePawnControlRotation = newState;
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

void AInGameCharacter::UpdateMeshes_Multicast_Implementation(FPlayersCharacterInfo CharacterInfo)
{
	UpdateMeshes(CharacterInfo.CharacterMeshData, CharacterInfo.CharacterColor);
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

