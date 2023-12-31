// Created by Jan Paraska. All rights reserved


#include "N3DNonogram.h"

#include "N3DGameInstance.h"
#include "N3DNonogramColorScheme.h"
#include "N3DNonogramInput.h"
#include "Nonogram3DTypes.h"

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
	
	Controller = PlayerController;

	if (NonogramInput)
	{
		if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			PlayerEnhancedInputComponent->ClearActionBindings();

			PlayerEnhancedInputComponent->BindAction(NonogramInput->SelectionX, ETriggerEvent::Started, this, &ThisClass::SelectionX);
			PlayerEnhancedInputComponent->BindAction(NonogramInput->SelectionY, ETriggerEvent::Started, this, &ThisClass::SelectionY);
			PlayerEnhancedInputComponent->BindAction(NonogramInput->SelectionZ, ETriggerEvent::Started, this, &ThisClass::SelectionZ);
			PlayerEnhancedInputComponent->BindAction(NonogramInput->SelectCube, ETriggerEvent::Started, this, &ThisClass::SelectCube);
		}
	}
}

void AN3DNonogram::SpawnCubeInstances()
{
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
}

void AN3DNonogram::SetSelectedColor(const FColor& NewColor)
{
	SelectedColor = Mode == EGameMode::Editor ? NewColor : FColor::White;
}

void AN3DNonogram::Resize(const FIntVector& Size)
{
	if (!ensure(Mode == EGameMode::Editor))
	{
		return;
	}

	CurrentSize = Size;
	SpawnCubeInstances();
	DeselectAllCubes();
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
		GameInstance->OnModeChanged.AddDynamic(this, &ThisClass::OnGameModeChanged);
	}

	// TODO get puzzle info on game start

	/// Temp, setup nonogram based on current puzzle info, move to OnGameModeChanged
	SetSolution(TestSolution);
	CurrentSize = FIntVector(10);
	SpawnCubeInstances();
	CurrentSelection = { ESelectionType::X, 0 };
	SelectPlane(CurrentSelection.Key, CurrentSelection.Value);
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

void AN3DNonogram::SelectCube()
{
	if (Controller && Controller->bShowMouseCursor)
	{
		FVector WorldLocation;
		FVector WorldDirection;
		if (Controller->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
		{
			const int32 FirstInstanceInSelectedPlane = *SelectionCollection[CurrentSelection.Key].Collection[CurrentSelection.Value].Plane.CreateIterator();
			FTransform PlaneBase;
			
			// This value will lead to errors, when selecting a cube from sharp angle
			// TODO offset the PlaneBase +-50.0f and check against two planes, then check which one is closer to mouse world position
			if (CubeInstances->GetInstanceTransform(FirstInstanceInSelectedPlane, PlaneBase, true))
			{
				FVector PlaneForward;
				switch (CurrentSelection.Key)
				{
				case ESelectionType::X:
					PlaneForward = FVector(1, 0, 0);
					break;
				case ESelectionType::Y:
					PlaneForward = FVector(0, 1, 0);
					break;
				case ESelectionType::Z:
					PlaneForward = FVector(0, 0, 1);
					break;
				default:
					return;
				}

				FPlane Plane = FPlane(PlaneBase.GetLocation(), PlaneForward);
				const FVector MousePoint = FMath::RayPlaneIntersection(WorldLocation, WorldDirection, Plane) / 100.0f;

				FIntVector InstanceCoords;
				InstanceCoords.X = FMath::RoundToInt(MousePoint.X);
				InstanceCoords.Y = FMath::RoundToInt(MousePoint.Y);
				InstanceCoords.Z = FMath::RoundToInt(MousePoint.Z);

				if (Cubes.Contains(InstanceCoords))
				{
					SelectCube(Cubes[InstanceCoords]);
				}
			}
		}
	}
}

void AN3DNonogram::OnGameModeChanged(const EGameMode NewMode)
{
	Mode = NewMode;

	switch (Mode)
	{
	case EGameMode::MainMenu:
		break;
	case EGameMode::Solving:
		break;
	case EGameMode::Editor:
		CurrentSolution.Empty();
		SelectedCubes.Empty();
		Resize(FIntVector(10));
		break;
	default:
		break;
	}
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
		SelectPlane(Selection, 0); // TODO remember selection on different axis?
	}
	else
	{
		int NextIndex = CurrentSelection.Value + (bNext ? 1 : -1);
		switch (Selection)
		{
		case ESelectionType::X:
			if (NextIndex < 0)
			{
				NextIndex = CurrentSize.X - 1;
			}
			else if (NextIndex >= CurrentSize.X)
			{
				NextIndex = 0;
			}
			break;
		case ESelectionType::Y:
			if (NextIndex < 0)
			{
				NextIndex = CurrentSize.Y - 1;
			}
			else if (NextIndex >= CurrentSize.Y)
			{
				NextIndex = 0;
			}
			break;
		case ESelectionType::Z:
			if (NextIndex < 0)
			{
				NextIndex = CurrentSize.Z - 1;
			}
			else if (NextIndex >= CurrentSize.Z)
			{
				NextIndex = 0;
			}
			break;
		}

		SelectPlane(CurrentSelection.Key, NextIndex);
	}
}

void AN3DNonogram::SelectPlane(const ESelectionType Selection, const int Index)
{
	// Deselect previouus
	const TArray<float> EmptyInactive = ColorScheme->EmptyInactive.Get();
	TArray<float> FilledInactive = ColorScheme->FilledInactive.Get();
	for (const int32 InstanceIndex : SelectionCollection[CurrentSelection.Key].Collection[CurrentSelection.Value].Plane)
	{
		if (Mode == EGameMode::Editor && CurrentSolution.Contains(InstanceIndex))
		{
			FilledInactive[0] = SelectedColor.R;
			FilledInactive[1] = SelectedColor.G;
			FilledInactive[2] = SelectedColor.B;
		}
		const bool bSelected = (Mode == EGameMode::Editor && CurrentSolution.Contains(InstanceIndex)) || (Mode == EGameMode::Solving && SelectedCubes.Contains(InstanceIndex));
		CubeInstances->SetCustomData(InstanceIndex, bSelected ? FilledInactive : EmptyInactive);
	}

	// Select new
	const TArray<float> EmptyActive = ColorScheme->EmptyActive.Get();
	TArray<float> FilledActive = ColorScheme->FilledActive.Get();
	for (const int32 InstanceIndex : SelectionCollection[Selection].Collection[Index].Plane)
	{
		if (Mode == EGameMode::Editor && CurrentSolution.Contains(InstanceIndex))
		{
			FilledActive[0] = SelectedColor.R;
			FilledActive[1] = SelectedColor.G;
			FilledActive[2] = SelectedColor.B;
		}
		const bool bSelected = (Mode == EGameMode::Editor && CurrentSolution.Contains(InstanceIndex)) || (Mode == EGameMode::Solving && SelectedCubes.Contains(InstanceIndex));
		CubeInstances->SetCustomData(InstanceIndex, bSelected ? FilledActive : EmptyActive);
	}

	CubeInstances->MarkRenderStateDirty();

	CurrentSelection = { Selection, Index };
}

void AN3DNonogram::SetSolution(UDataTable* SolutionDataTable)
{
	if (!ensure(SolutionDataTable))
	{
		return;
	}

	CurrentSolution.Empty();
	FString ContextString;
	TArray<FNonogramSolution*> SolutionRows;
	SolutionDataTable->GetAllRows<FNonogramSolution>(ContextString, SolutionRows);
	const TMap<FName, uint8*> Solution = SolutionDataTable->GetRowMap();
	for (const FNonogramSolution* SolutionRow : SolutionRows)
	{
		CurrentSolution.Add(SolutionRow->CubeIndex, SolutionRow->FinalColor);
	}
}

bool AN3DNonogram::CheckSolution() const
{
	if (SelectedCubes.Num() != CurrentSolution.Num())
	{
		return false;
	}

	for (const int32 InstanceIndex : SelectedCubes)
	{
		if (!CurrentSolution.Contains(InstanceIndex))
		{
			return false;
		}
	}

	return true;
}

void AN3DNonogram::DeselectAllCubes()
{
	const TArray<float> EmptyInactive = ColorScheme->EmptyInactive.Get();
	for (const TPair<FIntVector, int>& Cube : Cubes)
	{
		CubeInstances->SetCustomData(Cube.Value, EmptyInactive);
	}
	SelectPlane(ESelectionType::X, 0);
}

void AN3DNonogram::SelectCube(const int32 CubeIndex)
{
	switch (Mode)
	{
	case EGameMode::Solving:
		if (SelectedCubes.Contains(CubeIndex))
		{
			CubeInstances->SetCustomData(CubeIndex, ColorScheme->EmptyActive.Get());
			SelectedCubes.Remove(CubeIndex);
		}
		else
		{
			CubeInstances->SetCustomData(CubeIndex, ColorScheme->FilledActive.Get());
			SelectedCubes.Add(CubeIndex);
		}
		CubeInstances->MarkRenderStateDirty();

		if (CheckSolution())
		{
			UE_LOG(LogTemp, Warning, TEXT("Nonogram completed"));
		}
		break;
	case EGameMode::Editor:
		if (SelectedCubes.Contains(CubeIndex))
		{
			CubeInstances->SetCustomData(CubeIndex, ColorScheme->EmptyActive.Get());
			CurrentSolution.Remove(CubeIndex);
		}
		else
		{
			TArray<float> ColorData = ColorScheme->FilledActive.Get();
			ColorData[0] = SelectedColor.R;
			ColorData[1] = SelectedColor.G;
			ColorData[2] = SelectedColor.B;
			CubeInstances->SetCustomData(CubeIndex, ColorData);
			CurrentSolution.Add(CubeIndex, SelectedColor);
		}
		CubeInstances->MarkRenderStateDirty();
		break;
	default:
		break;
	}
}
