// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HauntedHouse/Global/GlobalStructs.h"
#include "BaseCharacterDataAsset.generated.h"

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

	FPlayersCharacterInfo ConvertToPlayersCharacterInfo() const;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
