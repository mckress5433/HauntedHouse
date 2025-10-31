// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "CharacterViewModel.generated.h"

/**
 * 
 */
UCLASS()
class HAUNTEDHOUSE_API UCharacterViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

	UCharacterViewModel();

protected:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 CurrentHealth;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 MaxHealth;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 CurrentStamina;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 MaxStamina;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 StaminaRegenRate;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 Strength;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 Speed;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 Intelligence;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 Sanity;
public:

private:
protected:

	UFUNCTION()
	void OnHealthChanged(int32 NewCurrentHealth);
	UFUNCTION()
	void OnMaxHealthChanged(int32 NewMaxHealth);
	UFUNCTION()
	void OnStaminaChanged(int32 NewCurrentStamina);
	UFUNCTION()
	void OnMaxStaminaChanged(int32 NewMaxStamina);
	UFUNCTION()
	void OnStaminaRegenRateChanged(int32 NewStaminaRegenRate);
	UFUNCTION()
	void OnStrengthChanged(int32 NewStrength);
	UFUNCTION()
	void OnSpeedChanged(int32 NewSpeed);
	UFUNCTION()
	void OnIntelligenceChanged(int32 NewIntelligence);
	UFUNCTION()
	void OnSanityChanged(int32 NewSanity);
	
public:

	void SetupListeners();
	
	void SetCurrentHealth(int32 NewCurrentHealth);
	void SetMaxHealth(int32 NewMaxHealth);
	void SetCurrentStamina(int32 NewCurrentStamina);
	void SetMaxStamina(int32 NewMaxStamina);
	void SetStaminaRegenRate(int32 NewStaminaRegenRate);
	void SetStrength(int32 NewStrength);
	void SetSpeed(int32 NewSpeed);
	void SetIntelligence(int32 NewIntelligence);
	void SetSanity(int32 NewSanity);

	int32 GetCurrentHealth() const { return CurrentHealth; }
	int32 GetMaxHealth() const { return MaxHealth; }
	int32 GetCurrentStamina() const { return CurrentStamina; }
	int32 GetMaxStamina() const { return MaxStamina; }
	int32 GetStaminaRegenRate() const { return StaminaRegenRate; }
	int32 GetStrength() const { return Strength; }
	int32 GetSpeed() const { return Speed; }
	int32 GetIntelligence() const { return Intelligence; }
	int32 GetSanity() const { return Sanity; }
	
	UFUNCTION(BlueprintPure, FieldNotify)
	float GetHealthPercent() const;
	UFUNCTION(BlueprintPure, FieldNotify)
	FString GetHealthText() const;

	UFUNCTION(BlueprintPure, FieldNotify)
	float GetStaminaPercent() const;
	UFUNCTION(BlueprintPure, FieldNotify)
	FString GetStaminaText() const;
	UFUNCTION(BlueprintPure, FieldNotify)
	FString GetStaminaRegenRateText() const;

	UFUNCTION(BlueprintPure, FieldNotify)
	FString GetStrengthText() const;
	UFUNCTION(BlueprintPure, FieldNotify)
	FString GetSpeedText() const;
	UFUNCTION(BlueprintPure, FieldNotify)
	FString GetIntelligenceText() const;
	UFUNCTION(BlueprintPure, FieldNotify)
	FString GetSanityText() const;
};
