// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Net/UnrealNetwork.h"

void ABaseCharacter::UpdateCameraRotationInterpolation()
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
}

FRotator ABaseCharacter::GetTargetCameraRotation() const
{
	if (CameraComp == nullptr || FocusTarget == nullptr) return FRotator::ZeroRotator;
	
	// Calculate the target rotation from the camera's position to the target position
	FVector CameraLocation = CameraComp->GetComponentLocation();
	FVector DirectionToTarget = (FocusTarget->GetActorLocation() - CameraLocation).GetSafeNormal();
	return FRotationMatrix::MakeFromX(DirectionToTarget).Rotator();
}

void ABaseCharacter::Server_SetUIActive_Implementation(bool bIsActive)
{
	bIsUIActive = bIsActive;
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, bIsUIActive);
}

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(FName("CameraComp"));
	CameraComp->SetupAttachment(GetMesh());

	WidgetInteractionComp = CreateDefaultSubobject<UWidgetInteractionComponent>(FName("WidgetInteractionComp"));
	WidgetInteractionComp->SetupAttachment(CameraComp);
}

void ABaseCharacter::HandleUIInteractionInput() const
{
	if (WidgetInteractionComp !=  nullptr && WidgetInteractionComp->IsActive())
	{
		WidgetInteractionComp->PressPointerKey(FKey(EKeys::LeftMouseButton));
		WidgetInteractionComp->ReleasePointerKey(FKey(EKeys::LeftMouseButton));
	}
}

void ABaseCharacter::StartCameraRotationInterpolation(AActor* LookAtTarget, float InterpolationDuration)
{
	if (CameraComp == nullptr || LookAtTarget == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("FAILED! StartCameraRotationInterpolation"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("StartCameraRotationInterpolation"));
	FocusTarget = LookAtTarget;

	InitialCameraRotation = CameraComp->GetComponentRotation();
	// Set the interpolation duration and reset elapsed time
	CameraInterpolationDuration = InterpolationDuration;
	ElapsedTimeDuringInterpolation = 0.0f;
	
	GetWorld()->GetTimerManager().SetTimer(
		CameraInterpolationTimerHandle,
		this,
		&ABaseCharacter::UpdateCameraRotationInterpolation,
		0.01f,   // Tick every 0.01 seconds
		true
	);
}

void ABaseCharacter::CancelCameraInterpolation()
{
	if (CameraComp == nullptr) return;

	// Stop the timer
	GetWorld()->GetTimerManager().ClearTimer(CameraInterpolationTimerHandle);
	CameraInterpolationTimerHandle.Invalidate();
	
	Controller->SetControlRotation(GetTargetCameraRotation());
}

void ABaseCharacter::ToggleWidgetInteractionActivation(bool bIsActive)
{
	WidgetInteractionComp->SetActive(bIsActive);
	Server_SetUIActive(bIsActive);
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

