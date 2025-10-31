// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "OptionsPhone.generated.h"

class UWidgetComponent;

UCLASS()
class HAUNTEDHOUSE_API AOptionsPhone : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditDefaultsOnly)
	UWidgetComponent* Widget;
	
public:	
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	AOptionsPhone();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UUserWidget* GetWidget() { return Widget->GetWidget(); }
};
