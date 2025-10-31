// Fill out your copyright notice in the Description page of Project Settings.


#include "FrontEndCharacter.h"

// Sets default values
AFrontEndCharacter::AFrontEndCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFrontEndCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFrontEndCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFrontEndCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

