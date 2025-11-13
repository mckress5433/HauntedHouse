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

	if (auto character = Cast<AInGameCharacter>(GetOwner()); character != nullptr)
	{
		if (AController* controller = character->GetController(); controller != nullptr)
		{
			if (controller->IsLocalPlayerController())
			{
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

