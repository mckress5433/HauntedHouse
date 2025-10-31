// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HauntedHouse/Components/InteractableComponent.h"
#include "LevelTransitionTirgger.generated.h"

UCLASS()
class HAUNTEDHOUSE_API ALevelTransitionTirgger : public AActor
{
	GENERATED_BODY()
	
	// Sets default values for this actor's properties
	ALevelTransitionTirgger();
	
protected:
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* InteractableMesh;
	UPROPERTY(EditDefaultsOnly)
	UInteractableComponent* InteractableComponent;
public:

	/*
	 * Functions
	 */
	
private:
	UFUNCTION()
	void OnInteractEvent();
protected:
	virtual void BeginPlay() override;
public:
	
};
