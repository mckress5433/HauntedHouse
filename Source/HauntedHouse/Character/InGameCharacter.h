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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UWidgetInteractionComponent* WidgetInteractionComp;

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

	// Whether or not the player has the options menu open. Used to trigger animation in the animBP
	UPROPERTY(Replicated)
	bool bIsUIActive = false;

	AActor* FocusTarget;
	// Timer handle for camera rotation interpolation
	FTimerHandle CameraInterpolationTimerHandle;
	// Initial camera rotation before interpolation
	FRotator InitialCameraRotation;
	// Interpolation duration
	float CameraInterpolationDuration;
	// Elapsed time during interpolation
	float ElapsedTimeDuringInterpolation;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
	UCameraComponent* CameraComp;
	
	/*
	 * Functions
	 */
private:

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetFirstPersonMesh(USkeletalMesh* NewMesh);
	void SetThirdPersonMesh(USkeletalMesh* NewMesh);
	
	UFUNCTION(Server, Reliable)
	void UpdateMeshes_Multicast();

	// Updates the camera rotation during interpolation
	void UpdateCameraRotationInterpolation();
	FRotator GetTargetCameraRotation();

	UFUNCTION(Server, Reliable)
	void Server_SetUIActive(bool bIsActive);

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsUIActive() { return bIsUIActive; }

	UInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }
	
	// Handles movement input
	void HandleMoveInput(const FVector2D& InputVector);
	// Handles the start of an interaction input
	void HandleInteractionInput_Start();
	// Handles the end of an interaction input
	void HandleInteractionInput_End();
	void HandleUIInteractionInput();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void UpdateMeshes(FCharacterMeshData CharacterMeshData, FColor MeshColor);

	void ToggleUsePawnControlRotation(bool newState);
	

	//function to start interpolation to look at a specific position
	UFUNCTION(BlueprintCallable)
	void StartCameraRotationInterpolation(AActor* LookAtTarget, float InterpolationDuration);
	// function to cancel the ongoing interpolation and reset the camera
	UFUNCTION(BlueprintCallable)
	void CancelCameraInterpolation();
	void ToggleWidgetInteractionActivation(bool bIsActive);
};