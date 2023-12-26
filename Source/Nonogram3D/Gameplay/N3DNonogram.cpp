// Created by Jan Paraska. All rights reserved


#include "N3DNonogram.h"

#include "N3DGameInstance.h"
#include "N3DNonogramInput.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"

AN3DNonogram::AN3DNonogram()
{
	PrimaryActorTick.bCanEverTick = false;

	CubeInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Cubes"));
	RootComponent = CubeInstances;
}

void AN3DNonogram::EnableInput(APlayerController* PlayerController)
{
	Super::EnableInput(PlayerController);

	if (NonogramInput)
	{
		if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			PlayerEnhancedInputComponent->BindAction(NonogramInput->SelectionX, ETriggerEvent::Started, this, &ThisClass::SelectionX);
			PlayerEnhancedInputComponent->BindAction(NonogramInput->SelectionY, ETriggerEvent::Started, this, &ThisClass::SelectionY);
			PlayerEnhancedInputComponent->BindAction(NonogramInput->SelectionZ, ETriggerEvent::Started, this, &ThisClass::SelectionZ);
		}
	}
}

void AN3DNonogram::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(CubeInstances))
	{
		return;
	}

	if (UN3DGameInstance* GameInstance = Cast<UN3DGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		GameInstance->SetActiveNonogram(this);
	}

	// TODO get puzzle info

	/// Temp, setup nonogram based on current puzzle info
	const int Size = 10;
	for (uint8 x = 0; x < Size; x++)
	{
		for (uint8 y = 0; y < Size; y++)
		{
			for (uint8 z = 0; z < Size; z++)
			{
				FTransform InstanceTransform(FRotator::ZeroRotator, FVector(x * 100.0f, y * 100.0f, z * 100.0f), CubeScale);
				int32 CubeIndex = CubeInstances->AddInstance(InstanceTransform, true);
				Cubes.Add(FIntVector(x, y, z), CubeIndex);
			}
		}
	}
}

void AN3DNonogram::SelectionX(const FInputActionValue& Input)
{
	UE_LOG(LogTemp, Warning, TEXT("SelectionX %s"), *Input.ToString());
}

void AN3DNonogram::SelectionY(const FInputActionValue& Input)
{
	UE_LOG(LogTemp, Warning, TEXT("SelectionY %s"), *Input.ToString());
}

void AN3DNonogram::SelectionZ(const FInputActionValue& Input)
{
	UE_LOG(LogTemp, Warning, TEXT("SelectionZ %s"), *Input.ToString());
}

