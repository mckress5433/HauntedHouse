// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BaseCharacterDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FCharacterAttributeData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float MaxHealth;
	UPROPERTY(EditAnywhere)
	float MaxStamina;
	UPROPERTY(EditAnywhere)
	float StaminaRegenRate;

	UPROPERTY(EditAnywhere)
	float BaseStrength;
	UPROPERTY(EditAnywhere)
	float BaseSpeed;
	UPROPERTY(EditAnywhere)
	float BaseIntelligence;
	UPROPERTY(EditAnywhere)
	float BaseSanity;
};

USTRUCT(BlueprintType)
struct FCharacterMeshData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	USkeletalMesh* FirstPersonMesh;

	UPROPERTY(EditAnywhere)
	USkeletalMesh* ThirdPersonMesh;
};

/**
 * 
 */
UCLASS(BlueprintType)
class HAUNTEDHOUSE_API UBaseCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Replicated)
	FColor CharacterColor;

	UPROPERTY(EditAnywhere, Replicated)
	FCharacterMeshData CharacterMeshData;
	
	UPROPERTY(EditAnywhere, Replicated)
	FCharacterAttributeData CharacterAttributeData;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FCharacterAttributeData GetCharacterAttributeData() const { return CharacterAttributeData; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FCharacterMeshData GetCharacterMeshData() { return CharacterMeshData; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FColor GetCharacterColor() { return CharacterColor; }

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
