// Created by Jan Paraska. All rights reserved


#include "N3DNonogram.h"

#include "N3DGameInstance.h"
#include "N3DNonogramColorScheme.h"
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
	CurrentSize = FIntVector(10);
	ResetSelectionCollection(CurrentSize);

	CubeInstances->ClearInstances();

	TArray<float> BaseColorScheme = ColorScheme->EmptyInactive.Get();

	for (uint8 x = 0; x < CurrentSize.X; x++)
	{
		for (uint8 y = 0; y < CurrentSize.Y; y++)
		{
			for (uint8 z = 0; z < CurrentSize.Z; z++)
			{
				FTransform InstanceTransform(FRotator::ZeroRotator, FVector(x * 100.0f, y * 100.0f, z * 100.0f), CubeScale);
				int32 CubeIndex = CubeInstances->AddInstance(InstanceTransform, true);
				
				Cubes.Add(FIntVector(x, y, z), CubeIndex);
				AddInstanceToCollection(FIntVector(x, y, z), CubeIndex);

				CubeInstances->SetCustomData(CubeIndex, BaseColorScheme);
			}
		}
	}

	CubeInstances->MarkRenderStateDirty();

	CurrentSelection = { ESelectionType::X, 0 };
	Select(CurrentSelection.Key, CurrentSelection.Value);
}

void AN3DNonogram::SelectionX(const FInputActionValue& Input)
{
	SelectNext(ESelectionType::X, Input.Get<float>() > 0);
}

void AN3DNonogram::SelectionY(const FInputActionValue& Input)
{
	SelectNext(ESelectionType::Y, Input.Get<float>() > 0);
}

void AN3DNonogram::SelectionZ(const FInputActionValue& Input)
{
	SelectNext(ESelectionType::Z, Input.Get<float>() > 0);
}

void AN3DNonogram::ResetSelectionCollection(const FIntVector& Size)
{
	SelectionCollection.Empty();
	
	SelectionCollection.Add(ESelectionType::X, CreateSelectionCollection(Size.X));
	SelectionCollection.Add(ESelectionType::Y, CreateSelectionCollection(Size.Y));
	SelectionCollection.Add(ESelectionType::Z, CreateSelectionCollection(Size.Z));
}

FSelectionCollection AN3DNonogram::CreateSelectionCollection(int Size)
{
	FSelectionCollection Selection;
	for (int i = 0; i < Size; i++)
	{
		Selection.Collection.Add(i, FSelectionPlane());
	}
	return Selection;
}

void AN3DNonogram::AddInstanceToCollection(const FIntVector& Index, const int InstanceIndex)
{
	SelectionCollection[ESelectionType::X].Collection[Index.X].Plane.Add(InstanceIndex);
	SelectionCollection[ESelectionType::Y].Collection[Index.Y].Plane.Add(InstanceIndex);
	SelectionCollection[ESelectionType::Z].Collection[Index.Z].Plane.Add(InstanceIndex);
}

void AN3DNonogram::SelectNext(const ESelectionType Selection, const bool bNext)
{
	if (CurrentSelection.Key != Selection)
	{
		Select(Selection, 0); // TODO remember selection on different axis?
	}
	else
	{
		int NextIndex = CurrentSelection.Value + (bNext ? 1 : -1);
		switch (Selection)
		{
		case ESelectionType::X:
			if (NextIndex <= 0)
			{
				NextIndex = CurrentSize.X - 1;
			}
			else if (NextIndex >= CurrentSize.X)
			{
				NextIndex = 0;
			}
			break;
		case ESelectionType::Y:
			if (NextIndex <= 0)
			{
				NextIndex = CurrentSize.Y - 1;
			}
			else if (NextIndex >= CurrentSize.Y)
			{
				NextIndex = 0;
			}
			break;
		case ESelectionType::Z:
			if (NextIndex <= 0)
			{
				NextIndex = CurrentSize.Z - 1;
			}
			else if (NextIndex >= CurrentSize.Z)
			{
				NextIndex = 0;
			}
			break;
		}

		Select(CurrentSelection.Key, NextIndex);
	}
}

void AN3DNonogram::Select(const ESelectionType Selection, const int Index)
{
	// Deselect previouus
	const TArray<float> EmptyInactive = ColorScheme->EmptyInactive.Get();
	for (int32 InstanceIndex : SelectionCollection[CurrentSelection.Key].Collection[CurrentSelection.Value].Plane)
	{
		CubeInstances->SetCustomData(InstanceIndex, EmptyInactive);
	}

	// Select new
	const TArray<float> EmptyActive = ColorScheme->EmptyActive.Get();
	for (int32 InstanceIndex : SelectionCollection[Selection].Collection[Index].Plane)
	{
		CubeInstances->SetCustomData(InstanceIndex, EmptyActive); // TODO filled cube have different data
	}

	CubeInstances->MarkRenderStateDirty();

	CurrentSelection = { Selection, Index };
}
