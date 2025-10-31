// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BaseCharacterDataAsset.h"
#include "GameFramework/PlayerState.h"
#include "HauntedHouse/GameplayAbilities/HauntedHouseGameplayAbility.h"
#include "HauntedHouse/Player/AbilitySystem/CharacterAbilitySystemComponent.h"
#include "HauntedHouse/Player/AbilitySystem/CharacterAttributeSet.h"
#include "InGamePlayerState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentHealthChanged, int32, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChanged, int32, NewMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentStaminaChanged, int32, NewStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxStaminaChanged, int32, NewMaxStamina);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaRegenRateChanged, int32, NewStaminaRegenRate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStrengthChanged, int32, NewStrength);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpeedChanged, int32, NewSpeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIntelligenceChanged, int32, NewIntelligence);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSanityChanged, int32, NewSanity);

USTRUCT(BlueprintType)
struct FBaseAbilities
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UHauntedHouseGameplayAbility> SprintAbility;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UHauntedHouseGameplayAbility> ToggleOptionsAbility;
	
};

UCLASS()
class HAUNTEDHOUSE_API AInGamePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

	AInGamePlayerState();
	
protected:

	UPROPERTY(BlueprintReadWrite, Replicated)
	UCharacterAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, Replicated)
	UCharacterAttributeSet* AttributeSet;

	UPROPERTY(EditDefaultsOnly, Replicated)
	UBaseCharacterDataAsset* CharacterDataAsset;
	
	FTimerHandle InitAttributesTimerHandle;

	UPROPERTY(Replicated)
	FString PlayerUsername;

	UPROPERTY(EditDefaultsOnly)
	FBaseAbilities BaseAbilities;
	
public:

	UPROPERTY()
	FOnCurrentHealthChanged OnCurrentHealthChangedDelegate;
	UPROPERTY()
	FOnMaxHealthChanged OnMaxHealthChangedDelegate;
	UPROPERTY()
	FOnCurrentStaminaChanged OnCurrentStaminaChangedDelegate;
	UPROPERTY()
	FOnMaxStaminaChanged OnMaxStaminaChangedDelegate;
	UPROPERTY()
	FOnStaminaRegenRateChanged OnStaminaRegenRateChangedDelegate;
	UPROPERTY()
	FOnStrengthChanged OnStrengthChangedDelegate;
	UPROPERTY()
	FOnSpeedChanged OnSpeedChangedDelegate;
	UPROPERTY()
	FOnIntelligenceChanged OnIntelligenceChangedDelegate;
	UPROPERTY()
	FOnSanityChanged OnSanityChangedDelegate;

	
	/*
	 * Functions
	 */

private:
	
	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void StaminaChanged(const FOnAttributeChangeData& Data);
	virtual void MaxStaminaChanged(const FOnAttributeChangeData& Data);
	virtual void StaminaRegenRateChanged(const FOnAttributeChangeData& Data);
	virtual void StrengthChanged(const FOnAttributeChangeData& Data);
	virtual void SpeedChanged(const FOnAttributeChangeData& Data);
	virtual void IntelligenceChanged(const FOnAttributeChangeData& Data);
	virtual void SanityChanged(const FOnAttributeChangeData& Data);
	
protected:
	
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetPlayerUsername() { return PlayerUsername; }
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void UpdateDefaultCharacterDataAsset(UBaseCharacterDataAsset* DataAsset);

	UBaseCharacterDataAsset* GetBaseCharacterDataAsset() { return CharacterDataAsset; }

	void InitializeAttributes(const FCharacterAttributeData CharacterAttributeData) const;

	UFUNCTION(BlueprintCallable)
	void UpdateCharacterInfoAndMeshes(UBaseCharacterDataAsset* DataAsset);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void HandleCharacterSelection(UBaseCharacterDataAsset* DataAsset);

	TSubclassOf<UHauntedHouseGameplayAbility> GetSprintAbilityClass() { return BaseAbilities.SprintAbility; }
	TSubclassOf<UHauntedHouseGameplayAbility> GetToggleOptionsAbilityClass() { return BaseAbilities.ToggleOptionsAbility; }
};
