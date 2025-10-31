// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "HauntedHouse/Player/PlayerState/BaseCharacterDataAsset.h"
#include "InGameCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCharacter, Log, All)

/**
 * 
 */
UCLASS()
class HAUNTEDHOUSE_API AInGameCharacter : public ABaseCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	AInGameCharacter();

	FTimerHandle InteractionZoneTickHandle;
	AActor* FocusedInteractable;

	// Console variable to toggle debug drawing
	static int32 bDrawDebug;
	static FAutoConsoleVariableRef CVarDrawDebug;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UWidgetInteractionComponent* WidgetInteractionComp;

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
	
	UPROPERTY(EditDefaultsOnly, Category=Interaction)
	float InteractionTickRate = 0.06f;
	// Trace radius for the sphere trace
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction)
	float SphereRadius = 100.0f;
	// Trace distance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction)
	float TraceDistance = 1000.0f;
	// Trace channel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interaction)
	TEnumAsByte<ECollisionChannel> TraceChannel;

	class UCharacterAbilitySystemComponent* AbilitySystemComponent;

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

	/*
	 * Functions
	 */
private:

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	void InteractionZoneTick();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetFirstPersonMesh(USkeletalMesh* NewMesh);
	void SetThirdPersonMesh(USkeletalMesh* NewMesh);
	
	UFUNCTION(Server, Reliable)
	void UpdateMeshes_Multicast();

	// Updates the camera rotation during interpolation
	void UpdateCameraRotationInterpolation();
	FRotator GetTargetCameraRotation();

public:
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