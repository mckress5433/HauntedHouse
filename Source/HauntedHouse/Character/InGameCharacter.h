// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "HauntedHouse/Components/InteractionComponent.h"
#include "HauntedHouse/Player/PlayerState/BaseCharacterDataAsset.h"
#include "InGameCharacter.generated.h"


/**
 * 
 */
UCLASS()
class HAUNTEDHOUSE_API AInGameCharacter : public ABaseCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	AInGameCharacter();
	
protected:
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category=Interaction)
	UInteractionComponent* InteractionComponent;

	UPROPERTY(EditDefaultsOnly)
	USkeletalMesh* FirstPersonMesh;
	UPROPERTY(EditDefaultsOnly)
	USkeletalMesh* ThirdPersonMesh;
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DynamicMaterialInstances;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAnimInstance> FirstPersonAnimBP;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAnimInstance> ThirdPersonAnimBP;
	

	UPROPERTY()
	class UCharacterAbilitySystemComponent* AbilitySystemComponent;
	
	
	/*
	 * Functions
	 */
private:

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	void SetFirstPersonMesh(USkeletalMesh* NewMesh);
	void SetThirdPersonMesh(USkeletalMesh* NewMesh);
	
	UFUNCTION(Server, Reliable)
	void UpdateMeshes_Multicast(FPlayersCharacterInfo CharacterInfo);

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsUIActive() { return bIsUIActive; }

	UInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }
	
	// Handles movement input
	void HandleMoveInput(const FVector2D& InputVector);
	// Handles the start of an interaction input
	void HandleInteractionInput_Start() const;
	// Handles the end of an interaction input
	void HandleInteractionInput_End() const;
	

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Updates the character meshes and materials. 
	void UpdateMeshes(const FCharacterMeshData& CharacterMeshData, const FColor& MeshColor);

	void ToggleUsePawnControlRotation(bool newState) const;
};