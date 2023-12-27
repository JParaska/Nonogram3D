// Created by Jan Paraska. All rights reserved


#include "N3DPlayerController.h"

#include "N3DGameInstance.h"
#include "N3DPlayerInput.h"
#include "N3DNonogram.h"

#include "InputTriggers.h"
#include "EnhancedInputComponent.h"
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

	if (PlayerInputConfig)
	{
		if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			PlayerEnhancedInputComponent->BindAction(PlayerInputConfig->TriggerLookAround, ETriggerEvent::Started, this, &ThisClass::TriggerLookAround);
			PlayerEnhancedInputComponent->BindAction(PlayerInputConfig->TriggerLookAround, ETriggerEvent::Completed, this, &ThisClass::TriggerLookAround);
			PlayerEnhancedInputComponent->BindAction(PlayerInputConfig->TriggerLookAround, ETriggerEvent::Canceled, this, &ThisClass::TriggerLookAround);

			PlayerEnhancedInputComponent->BindAction(PlayerInputConfig->LookAround, ETriggerEvent::Triggered, this, &ThisClass::LookAround);
		}
	}
}

void AN3DPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UN3DGameInstance* GameInstance = Cast<UN3DGameInstance>(UGameplayStatics::GetGameInstance(this))) {
		if (GameInstance->GetActiveNonogram() && !GameInstance->GetActiveNonogram()->IsActorBeingDestroyed())
		{
			GameInstance->GetActiveNonogram()->EnableInput(this);
		}
		else
		{
			GameInstance->OnNonogramSet.BindUObject(this, &ThisClass::OnNonogramSet);
		}
	}

	bShowMouseCursor = true;
}

void AN3DPlayerController::TriggerLookAround(const FInputActionValue& Input)
{
	bShowMouseCursor = !Input.Get<bool>();
}

void AN3DPlayerController::LookAround(const FInputActionValue& Input)
{
	UE_LOG(LogTemp, Warning, TEXT("Look around %s"), *Input.ToString());
	if (Input.GetMagnitude() > 0 && !bShowMouseCursor)
	{
		const FVector2D Direction = Input.Get<FVector2D>();
		AddYawInput(Direction.X);
		AddPitchInput(-Direction.Y);
	}
}

void AN3DPlayerController::OnNonogramSet(AN3DNonogram* Nonogram)
{
	if (ensure(Nonogram))
	{
		Nonogram->EnableInput(this);
	}
}
