// Fill out your copyright notice in the Description page of Project Settings.


#include "OptionsPhone.h"

#include "InGameCharacter.h"
#include "Components/Widget.h"
#include "Components/WidgetComponent.h"

// Sets default values
AOptionsPhone::AOptionsPhone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;

	Widget = CreateDefaultSubobject<UWidgetComponent>("Widget");
	Widget->SetupAttachment(Mesh);
	Widget->SetAbsolute(false, false, false);
}

// Called when the game starts or when spawned
void AOptionsPhone::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay phone"));
	if (auto character = Cast<ABaseCharacter>(GetOwner()); character != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character is %s"), *character->GetName());
		if (AController* controller = character->GetController(); controller != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Controller valid"));
			if (controller->IsLocalPlayerController())
			{
				UE_LOG(LogTemp, Warning, TEXT("Pls Start"));
				character->StartCameraRotationInterpolation(this, 0.5f);
			}
		}
	}
}

// Called every frame
void AOptionsPhone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOptionsPhone::SetWidgetClass(TSubclassOf<UUserWidget> WidgetClass)
{
	Widget->SetWidgetClass(WidgetClass);
	Widget->InitWidget();
}
