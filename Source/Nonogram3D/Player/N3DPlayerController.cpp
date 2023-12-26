// Created by Jan Paraska. All rights reserved


#include "N3DPlayerController.h"

#include "N3DGameInstance.h"
#include "N3DPlayerInput.h"
#include "Nonogram.h"

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

			PlayerEnhancedInputComponent->BindAction(PlayerInputConfig->SelectCube, ETriggerEvent::Started, this, &ThisClass::SelectCube);

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
}

void AN3DPlayerController::TriggerLookAround(const FInputActionValue& Input)
{
	UE_LOG(LogTemp, Warning, TEXT("Trigger look around %s"), *Input.ToString());
}

void AN3DPlayerController::SelectCube()
{
	UE_LOG(LogTemp, Warning, TEXT("Input pressed to select cude under cursor from selected plane"));
}

void AN3DPlayerController::LookAround(const FInputActionValue& Input)
{
	UE_LOG(LogTemp, Warning, TEXT("Look around %s"), *Input.ToString());
	if (Input.GetMagnitude() > 0)
	{
		const FVector2D Direction = Input.Get<FVector2D>();
		AddYawInput(Direction.X);
		AddPitchInput(-Direction.Y);
	}
}

void AN3DPlayerController::OnNonogramSet(ANonogram* Nonogram)
{
	if (ensure(Nonogram))
	{
		Nonogram->EnableInput(this);
	}
}
