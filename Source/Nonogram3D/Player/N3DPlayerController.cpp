// Created by Jan Paraska. All rights reserved


#include "N3DPlayerController.h"

#include "N3DNonogram.h"

#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

void AN3DPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (PlayerInputMappingContext)
	{
		if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				InputSystem->AddMappingContext(PlayerInputMappingContext, 0);
			}
		}
	}
}

void AN3DPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
}
