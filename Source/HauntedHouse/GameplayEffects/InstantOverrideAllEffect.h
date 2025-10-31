// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "InstantOverrideAllEffect.generated.h"

/**
 * 
 */
UCLASS()
class HAUNTEDHOUSE_API UInstantOverrideAllEffect : public UGameplayEffect
{
	GENERATED_BODY()

	UInstantOverrideAllEffect();
	FGameplayModifierInfo CreateModifierInfo(FGameplayAttribute GameplayAttribute, FString TagString);
	FSetByCallerFloat CreateSetByCallerFloat(FString TagString);
};
