// Fill out your copyright notice in the Description page of Project Settings.


#include "InGamePlayerController.h"
#include "HauntedHouse/Character/InGameCharacter.h"
#include "PlayerState/InGamePlayerState.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"

void AInGamePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem
		= ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
#if UE_EDITOR
		Subsystem->AddMappingContext(EditorMappingContext, 0);
#else
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
#endif
		
	}
}

void AInGamePlayerController::Move(const FInputActionValue& Value)
{
	AInGameCharacter* PlayerCharacter = Cast<AInGameCharacter>(GetCharacter());
	if (PlayerCharacter)
	{
		// Pass the input to the player character
		PlayerCharacter->HandleMoveInput(Value.Get<FVector2D>());
	}
}

void AInGamePlayerController::Look(const FInputActionValue& Value)
{
	// Get the input as a 2D vector (X = yaw, Y = pitch)
	const FVector2D LookInput = Value.Get<FVector2D>();

	// Apply yaw (look left/right) and pitch (look up/down)
	AddYawInput(LookInput.X);
	AddPitchInput(LookInput.Y);
}

void AInGamePlayerController::StartInteraction()
{
	AInGameCharacter* PlayerCharacter = Cast<AInGameCharacter>(GetCharacter());
	if (PlayerCharacter)
	{
		// Pass the input to the player character
		PlayerCharacter->HandleInteractionInput_Start();
	}
}

void AInGamePlayerController::EndInteraction() 
{
	AInGameCharacter* PlayerCharacter = Cast<AInGameCharacter>(GetCharacter());
	if (PlayerCharacter != nullptr)
	{
		// Pass the input to the player character
		PlayerCharacter->HandleInteractionInput_End();
	}
}

void AInGamePlayerController::ToggleOptionsMenu()
{
	if (auto PS = GetPlayerState<AInGamePlayerState>(); PS != nullptr)
	{
		if (auto ASC = PS->GetAbilitySystemComponent(); ASC != nullptr)
		{
			// Find the sprint ability spec and activate it
			FGameplayAbilitySpec* abilitySpec = ASC->FindAbilitySpecFromClass(PS->GetToggleOptionsAbilityClass());
			if (abilitySpec != nullptr)
			{
				if (abilitySpec->IsActive())
				{
					ASC->CancelAbilityHandle(abilitySpec->Handle);
					if (auto inGamePawn = Cast<AInGameCharacter>(GetPawn()); inGamePawn != nullptr)
					{
						inGamePawn->ToggleWidgetInteractionActivation(false);
					}
				}
				else
				{
					if (!ASC->TryActivateAbility(abilitySpec->Handle))
					{
						UE_LOG(LogController, Warning, TEXT("Failed to activate options menu"));
					}
					else
					{
						if (auto inGamePawn = Cast<AInGameCharacter>(GetPawn()); inGamePawn != nullptr)
						{
							inGamePawn->ToggleWidgetInteractionActivation(true);
						}
					}
				}
			}
		}
	}
}

void AInGamePlayerController::OnUIInteraction()
{
	if (IsPauseMenuOpen())
	{
		if (auto inGameCharacter = Cast<AInGameCharacter>(GetPawn()); inGameCharacter != nullptr)
		{
			inGameCharacter->HandleUIInteractionInput();
		}
	}
}

void AInGamePlayerController::SetUIInputMode()
{
	// Show the mouse cursor
	bShowMouseCursor = true;
	// Set input mode to Game and UI (this allows keyboard inputs to affect the game while interacting with UI)
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
	// Disable camera control while interacting with the UI
	SetIgnoreLookInput(true);

	if(auto inGamePawn = Cast<AInGameCharacter>(GetPawn()); inGamePawn != nullptr)
	{
		inGamePawn->ToggleUsePawnControlRotation(false);
	}
}

void AInGamePlayerController::SetGameInputMode()
{
	// Hide the mouse cursor
	bShowMouseCursor = false;
	// Set input mode back to Game only
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	// Re-enable camera control
	SetIgnoreLookInput(false);

	if(auto inGamePawn = Cast<AInGameCharacter>(GetPawn()); inGamePawn != nullptr)
	{
		inGamePawn->ToggleUsePawnControlRotation(true);
	}
}

void AInGamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Ensure the input component is an EnhancedInputComponent
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Bind Move action
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);

		// Bind Look action (for camera control)
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

		// Bind Interact action (Pressed and released)
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::StartInteraction);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &ThisClass::EndInteraction);

		// Bind Options menu toggling
		EnhancedInputComponent->BindAction(ToggleOptionsMenuAction, ETriggerEvent::Started, this, &ThisClass::ToggleOptionsMenu);

		EnhancedInputComponent->BindAction(UIInteractionAction, ETriggerEvent::Triggered, this, &ThisClass::OnUIInteraction);
	}
}

bool AInGamePlayerController::IsPauseMenuOpen() const
{
	if (auto PS = GetPlayerState<AInGamePlayerState>(); PS != nullptr)
	{
		if (auto ASC = PS->GetAbilitySystemComponent(); ASC != nullptr)
		{
			// Find the sprint ability spec and activate it
			FGameplayAbilitySpec* abilitySpec = ASC->FindAbilitySpecFromClass(PS->GetToggleOptionsAbilityClass());
			if (abilitySpec != nullptr)
			{
				return abilitySpec->IsActive();
			}
		}
	}
	return false;
}

