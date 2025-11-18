#pragma once
#include "CoreMinimal.h"
#include "HauntedHouse/Player/PlayerState/BaseCharacterDataAsset.h"
#include "GlobalStructs.generated.h"

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

USTRUCT(BlueprintType)
struct FPlayersCharacterInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FColor CharacterColor;

	UPROPERTY(EditAnywhere)
	FCharacterMeshData CharacterMeshData;
	
	UPROPERTY(EditAnywhere)
	FCharacterAttributeData CharacterAttributeData;

	FPlayersCharacterInfo()
	{
		CharacterColor = FColor::White;
		CharacterMeshData = FCharacterMeshData();
		CharacterAttributeData = FCharacterAttributeData();
	}
	
	FPlayersCharacterInfo(FColor InCharacterColor, FCharacterMeshData InCharacterMeshData, FCharacterAttributeData InCharacterAttributeData)
	: CharacterColor(InCharacterColor), CharacterMeshData(InCharacterMeshData), CharacterAttributeData(InCharacterAttributeData)
	{
	}
};

USTRUCT(BlueprintType)
struct FSessionSaveStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FPlayersCharacterInfo CharacterInfo;

	UPROPERTY(EditAnywhere)
	int SessionId;

	FSessionSaveStruct()
	{
		CharacterInfo = FPlayersCharacterInfo();
		SessionId = 0;
	}
	FSessionSaveStruct(FPlayersCharacterInfo InCharacterInfo, int InSessionId)
	: CharacterInfo(InCharacterInfo), SessionId(InSessionId)
	{
	}
};