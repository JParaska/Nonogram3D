// Created by Jan Paraska. All rights reserved


#include "N3DPawn.h"

#include "N3DNonogram.h"
#include "N3DPlayerInput.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "InputTriggers.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"

AN3DPawn::AN3DPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = (MaxSpingArmLength - MinSpringArmLength) / 2.0f;
	DesiredCameraZoom = SpringArm->TargetArmLength;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void AN3DPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpringArm->TargetArmLength = FMath::Lerp(SpringArm->TargetArmLength, DesiredCameraZoom, CameraZoomSpeed * DeltaTime);
}

void AN3DPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (PlayerInputConfig)
	{
		if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
		{
			PlayerEnhancedInputComponent->ClearActionBindings();

			PlayerEnhancedInputComponent->BindAction(PlayerInputConfig->TriggerLookAround, ETriggerEvent::Started, this, &ThisClass::TriggerLookAround);
			PlayerEnhancedInputComponent->BindAction(PlayerInputConfig->TriggerLookAround, ETriggerEvent::Completed, this, &ThisClass::TriggerLookAround);
			PlayerEnhancedInputComponent->BindAction(PlayerInputConfig->TriggerLookAround, ETriggerEvent::Canceled, this, &ThisClass::TriggerLookAround);

			PlayerEnhancedInputComponent->BindAction(PlayerInputConfig->LookAround, ETriggerEvent::Triggered, this, &ThisClass::LookAround);

			PlayerEnhancedInputComponent->BindAction(PlayerInputConfig->CameraZoom, ETriggerEvent::Triggered, this, &ThisClass::CameraZoom);
		}
	}

	if (APlayerController* PC = CastChecked<APlayerController>(Controller))
	{
		if (Nonogram)
		{
			Nonogram->EnableInput(PC);
		}
	}
}

void AN3DPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void AN3DPawn::TriggerLookAround(const FInputActionValue& Input)
{
	if (APlayerController* PC = CastChecked<APlayerController>(Controller))
	{
		PC->bShowMouseCursor = !Input.Get<bool>();
	}
}

void AN3DPawn::LookAround(const FInputActionValue& Input)
{
	if (APlayerController* PC = CastChecked<APlayerController>(Controller))
	{
		if (Input.GetMagnitude() > 0 && !PC->bShowMouseCursor)
		{
			const FVector2D Direction = Input.Get<FVector2D>();
			AddControllerYawInput(Direction.X);
			AddControllerPitchInput(-Direction.Y);
		}
	}
}

void AN3DPawn::CameraZoom(const FInputActionValue& Input)
{
	const float Direction = Input.Get<float>();
	DesiredCameraZoom -= Direction > 0 ? CameraZoomStep : -CameraZoomStep;
	DesiredCameraZoom = FMath::Clamp(DesiredCameraZoom, MinSpringArmLength, MaxSpingArmLength);
}
