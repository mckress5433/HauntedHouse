#pragma once
#include "CoreMinimal.h"
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
struct FSessionSaveStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FColor CharacterColor;

	UPROPERTY(EditAnywhere)
	FCharacterMeshData CharacterMeshData;
	
	UPROPERTY(EditAnywhere)
	FCharacterAttributeData CharacterAttributeData;
};