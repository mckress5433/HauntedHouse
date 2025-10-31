// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterViewModel.h"

#include "HauntedHouse/Player/PlayerState/InGamePlayerState.h"

UCharacterViewModel::UCharacterViewModel()
{
	if(const UWorld* world = GetWorld(); world != nullptr)
	{
		// Delay so clients view models are hooked up properly. Easy out because I don't want to try too hard
		FTimerHandle timerHandle;
		world->GetTimerManager().SetTimer(timerHandle, this, &ThisClass::SetupListeners, 0.2f, false);
	}
}

void UCharacterViewModel::OnHealthChanged(int32 NewCurrentHealth)
{
	SetCurrentHealth(NewCurrentHealth);
}

void UCharacterViewModel::OnMaxHealthChanged(int32 NewMaxHealth)
{
	SetMaxHealth(NewMaxHealth);
}

void UCharacterViewModel::OnStaminaChanged(int32 NewCurrentStamina)
{
	SetCurrentStamina(NewCurrentStamina);
}

void UCharacterViewModel::OnMaxStaminaChanged(int32 NewMaxStamina)
{
	SetMaxStamina(NewMaxStamina);
}

void UCharacterViewModel::OnStaminaRegenRateChanged(int32 NewStaminaRegenRate)
{
	SetStaminaRegenRate(NewStaminaRegenRate);
}

void UCharacterViewModel::OnStrengthChanged(int32 NewStrength)
{
	SetStrength(NewStrength);
}

void UCharacterViewModel::OnSpeedChanged(int32 NewSpeed)
{
	SetSpeed(NewSpeed);
}

void UCharacterViewModel::OnIntelligenceChanged(int32 NewIntelligence)
{
	SetIntelligence(NewIntelligence);
}

void UCharacterViewModel::OnSanityChanged(int32 NewSanity)
{
	SetSanity(NewSanity);
}

void UCharacterViewModel::SetupListeners()
{
	if(UWorld* world = GetWorld(); world != nullptr)
	{
		if(APlayerController* playerController = world->GetFirstPlayerController(); playerController != nullptr)
		{
			if(auto playerState = playerController->GetPlayerState<AInGamePlayerState>(); playerState != nullptr)
			{
				playerState->OnCurrentHealthChangedDelegate.AddDynamic(this, &ThisClass::OnHealthChanged);
				playerState->OnMaxHealthChangedDelegate.AddDynamic(this, &ThisClass::OnMaxHealthChanged);
				playerState->OnCurrentStaminaChangedDelegate.AddDynamic(this, &ThisClass::OnStaminaChanged);
				playerState->OnMaxStaminaChangedDelegate.AddDynamic(this, &ThisClass::OnMaxStaminaChanged);
				playerState->OnStaminaRegenRateChangedDelegate.AddDynamic(this, &ThisClass::OnStaminaRegenRateChanged);
				playerState->OnStrengthChangedDelegate.AddDynamic(this, &ThisClass::OnStrengthChanged);
				playerState->OnSpeedChangedDelegate.AddDynamic(this, &ThisClass::OnSpeedChanged);
				playerState->OnIntelligenceChangedDelegate.AddDynamic(this, &ThisClass::OnIntelligenceChanged);
				playerState->OnSanityChangedDelegate.AddDynamic(this, &ThisClass::OnSanityChanged);
				
				bool wasFound = false;
				SetCurrentHealth(playerState->GetAbilitySystemComponent()->GetGameplayAttributeValue(UCharacterAttributeSet::GetHealthAttribute(), wasFound));
				SetMaxHealth(playerState->GetAbilitySystemComponent()->GetGameplayAttributeValue(UCharacterAttributeSet::GetMaxHealthAttribute(), wasFound));
				SetCurrentStamina(playerState->GetAbilitySystemComponent()->GetGameplayAttributeValue(UCharacterAttributeSet::GetStaminaAttribute(), wasFound));
				SetMaxStamina(playerState->GetAbilitySystemComponent()->GetGameplayAttributeValue(UCharacterAttributeSet::GetMaxStaminaAttribute(), wasFound));
				SetCurrentStamina(playerState->GetAbilitySystemComponent()->GetGameplayAttributeValue(UCharacterAttributeSet::GetStaminaAttribute(), wasFound));
				SetStaminaRegenRate(playerState->GetAbilitySystemComponent()->GetGameplayAttributeValue(UCharacterAttributeSet::GetStaminaRegenRateAttribute(), wasFound));
				SetStrength(playerState->GetAbilitySystemComponent()->GetGameplayAttributeValue(UCharacterAttributeSet::GetStrengthAttribute(), wasFound));
				SetSpeed(playerState->GetAbilitySystemComponent()->GetGameplayAttributeValue(UCharacterAttributeSet::GetSpeedAttribute(), wasFound));
				SetIntelligence(playerState->GetAbilitySystemComponent()->GetGameplayAttributeValue(UCharacterAttributeSet::GetIntelligenceAttribute(), wasFound));
				SetSanity(playerState->GetAbilitySystemComponent()->GetGameplayAttributeValue(UCharacterAttributeSet::GetSanityAttribute(), wasFound));
			}
		}
	}
}

void UCharacterViewModel::SetCurrentHealth(int32 NewCurrentHealth)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(CurrentHealth, NewCurrentHealth))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHealthText);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHealthPercent);
	}
}

void UCharacterViewModel::SetMaxHealth(int32 NewMaxHealth)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(MaxHealth, NewMaxHealth))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHealthText);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHealthPercent);
	}
}

void UCharacterViewModel::SetCurrentStamina(int32 NewCurrentStamina)
{
	if(UE_MVVM_SET_PROPERTY_VALUE(CurrentStamina, NewCurrentStamina))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStaminaText);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStaminaPercent);
	}
}

void UCharacterViewModel::SetMaxStamina(int32 NewMaxStamina)
{
	if(UE_MVVM_SET_PROPERTY_VALUE(MaxStamina, NewMaxStamina))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStaminaText);
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStaminaPercent);
	}
}

void UCharacterViewModel::SetStaminaRegenRate(int32 NewStaminaRegenRate)
{
	if(UE_MVVM_SET_PROPERTY_VALUE(StaminaRegenRate, NewStaminaRegenRate))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStaminaRegenRateText);
	}
}

void UCharacterViewModel::SetStrength(int32 NewStrength)
{
	if(UE_MVVM_SET_PROPERTY_VALUE(Strength, NewStrength))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetStrengthText);
	}
}

void UCharacterViewModel::SetSpeed(int32 NewSpeed)
{
	if(UE_MVVM_SET_PROPERTY_VALUE(Speed, NewSpeed))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetSpeedText);
	}
}

void UCharacterViewModel::SetIntelligence(int32 NewIntelligence)
{
	if(UE_MVVM_SET_PROPERTY_VALUE(Intelligence, NewIntelligence))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetIntelligenceText);
	}
}

void UCharacterViewModel::SetSanity(int32 NewSanity)
{
	if(UE_MVVM_SET_PROPERTY_VALUE(Sanity, NewSanity))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetSanityText);
	}
}

float UCharacterViewModel::GetHealthPercent() const
{
	if(MaxHealth != 0)
	{
		return static_cast<float>(CurrentHealth) / static_cast<float>(MaxHealth);
	}
	else
	{
		return 0;
	}
}

FString UCharacterViewModel::GetHealthText() const
{
	return FString::Printf(TEXT("%d/%d"), CurrentHealth, MaxHealth);
}

float UCharacterViewModel::GetStaminaPercent() const
{
	if(MaxStamina != 0)
	{
		return  static_cast<float>(CurrentStamina) / static_cast<float>(MaxStamina);
	}
	else
	{
		return 0;
	}
}

FString UCharacterViewModel::GetStaminaText() const
{
	return FString::Printf(TEXT("%d/%d"), CurrentStamina, MaxStamina);
}

FString UCharacterViewModel::GetStaminaRegenRateText() const
{
	return FString::Printf(TEXT("StaminaRegenRate: %d"), StaminaRegenRate);
}

FString UCharacterViewModel::GetStrengthText() const
{
	return FString::Printf(TEXT("Strength: %d"), Strength);
}

FString UCharacterViewModel::GetSpeedText() const
{
	return FString::Printf(TEXT("Speed: %d"), Speed);
}

FString UCharacterViewModel::GetIntelligenceText() const
{
	return FString::Printf(TEXT("Intelligence: %d"), Intelligence);
}

FString UCharacterViewModel::GetSanityText() const
{
	return FString::Printf(TEXT("Sanity: %d"), Sanity);
}

