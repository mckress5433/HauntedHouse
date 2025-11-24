// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "BaseCharacter.generated.h"

UCLASS()
class HAUNTEDHOUSE_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
	TObjectPtr<UCameraComponent> CameraComp;
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetInteractionComponent> WidgetInteractionComp;
	
	AActor* FocusTarget;
	// Timer handle for camera rotation interpolation
	FTimerHandle CameraInterpolationTimerHandle;
	// Initial camera rotation before interpolation
	FRotator InitialCameraRotation;
	// Interpolation duration
	float CameraInterpolationDuration;
	// Elapsed time during interpolation
	float ElapsedTimeDuringInterpolation;

	// Whether or not the player has the options menu open. Used to trigger animation in the animBP
	UPROPERTY(Replicated)
	bool bIsUIActive = false;

	// Updates the camera rotation during interpolation
	void UpdateCameraRotationInterpolation();
	FRotator GetTargetCameraRotation() const;

	UFUNCTION(Server, Reliable)
	void Server_SetUIActive(bool bIsActive);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	void HandleUIInteractionInput() const;

	//function to start interpolation to look at a specific position
	UFUNCTION(BlueprintCallable)
	void StartCameraRotationInterpolation(AActor* LookAtTarget, float InterpolationDuration);
	// function to cancel the ongoing interpolation and reset the camera
	UFUNCTION(BlueprintCallable)
	void CancelCameraInterpolation();
	void ToggleWidgetInteractionActivation(bool bIsActive);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};


