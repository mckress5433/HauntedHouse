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
	int32 Health;
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 MaxHealth;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter)
	int32 Stamina;
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

	void SetupListeners();

	UFUNCTION()
	void SetHealth(const int32 NewHealth);
	UFUNCTION()
	void SetMaxHealth(int32 NewMaxHealth);
	UFUNCTION()
	void SetStamina(const int32 NewStamina);
	UFUNCTION()
	void SetMaxStamina(int32 NewMaxStamina);
	UFUNCTION()
	void SetStaminaRegenRate(int32 NewStaminaRegenRate);
	UFUNCTION()
	void SetStrength(int32 NewStrength);
	UFUNCTION()
	void SetSpeed(int32 NewSpeed);
	UFUNCTION()
	void SetIntelligence(int32 NewIntelligence);
	UFUNCTION()
	void SetSanity(int32 NewSanity);

	int32 GetHealth() const { return Health; }
	int32 GetMaxHealth() const { return MaxHealth; }
	int32 GetStamina() const { return Stamina; }
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
