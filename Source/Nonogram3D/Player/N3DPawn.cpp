// Created by Jan Paraska. All rights reserved


#include "N3DPawn.h"


AN3DPawn::AN3DPawn()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AN3DPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void AN3DPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

