// Fill out your copyright notice in the Description page of Project Settings.


#include "InstantOverrideAllEffect.h"

#include "HauntedHouse/Player/AbilitySystem/CharacterAttributeSet.h"

UInstantOverrideAllEffect::UInstantOverrideAllEffect()
{
	
	DurationPolicy = EGameplayEffectDurationType::Instant;
	
	Modifiers.Add(CreateModifierInfo(UCharacterAttributeSet::GetHealthAttribute(), "SetByCaller.Health"));
	Modifiers.Add(CreateModifierInfo(UCharacterAttributeSet::GetMaxHealthAttribute(), "SetByCaller.MaxHealth"));
	Modifiers.Add(CreateModifierInfo(UCharacterAttributeSet::GetStaminaAttribute(), "SetByCaller.Stamina"));
	Modifiers.Add(CreateModifierInfo(UCharacterAttributeSet::GetMaxStaminaAttribute(), "SetByCaller.MaxStamina"));
	Modifiers.Add(CreateModifierInfo(UCharacterAttributeSet::GetStaminaRegenRateAttribute(), "SetByCaller.StaminaRegenRate"));
	Modifiers.Add(CreateModifierInfo(UCharacterAttributeSet::GetStrengthAttribute(), "SetByCaller.Strength"));
	Modifiers.Add(CreateModifierInfo(UCharacterAttributeSet::GetSpeedAttribute(), "SetByCaller.Speed"));
	Modifiers.Add(CreateModifierInfo(UCharacterAttributeSet::GetIntelligenceAttribute(), "SetByCaller.Intelligence"));
	Modifiers.Add(CreateModifierInfo(UCharacterAttributeSet::GetSanityAttribute(), "SetByCaller.Sanity"));
}

FGameplayModifierInfo UInstantOverrideAllEffect::CreateModifierInfo(FGameplayAttribute GameplayAttribute,
	FString TagString)
{
	FGameplayModifierInfo HealthModifierInfo;
	HealthModifierInfo.Attribute = GameplayAttribute;
	HealthModifierInfo.ModifierOp = EGameplayModOp::Override;
	HealthModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(CreateSetByCallerFloat(TagString));

	return HealthModifierInfo;
}

FSetByCallerFloat UInstantOverrideAllEffect::CreateSetByCallerFloat(FString TagString)
{
	FSetByCallerFloat SetByCallerFloat;
	SetByCallerFloat.DataName = FName(UCharacterAttributeSet::GetHealthAttribute().AttributeName);
	SetByCallerFloat.DataTag = FGameplayTag::RequestGameplayTag(FName(TagString));

	return SetByCallerFloat;
}

