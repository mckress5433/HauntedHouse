// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HauntedHouse/Components/InteractableComponent.h"
#include "LevelTransitionTrigger.generated.h"

UCLASS()
class HAUNTEDHOUSE_API ALevelTransitionTrigger : public AActor
{
	GENERATED_BODY()
	
	// Sets default values for this actor's properties
	ALevelTransitionTrigger();
	
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
