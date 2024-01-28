// Created by Jan Paraska. All rights reserved


#include "N3DNonogram.h"

#include "N3DGameInstance.h"
#include "N3DNonogramColorScheme.h"
#include "N3DNonogramInput.h"
#include "N3DSaveSubsystem.h"
#include "N3DStatics.h"
#include "Nonogram3DTypes.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"


AN3DNonogram::AN3DNonogram()
{
	PrimaryActorTick.bCanEverTick = false;

	CubeInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Cubes"));
	RootComponent = CubeInstances;

	NonogramKeyWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("NonogramKey"));
	NonogramKeyWidget->SetupAttachment(RootComponent);

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

			PlayerEnhancedInputComponent->BindAction(NonogramInput->HighlightAllSelectedCubes, ETriggerEvent::Started, this, &ThisClass::HighlightAllSelectedCubes);
			PlayerEnhancedInputComponent->BindAction(NonogramInput->HighlightAllSelectedCubes, ETriggerEvent::Completed, this, &ThisClass::EndHighlight);
			PlayerEnhancedInputComponent->BindAction(NonogramInput->HighlightAllSelectedCubes, ETriggerEvent::Canceled, this, &ThisClass::EndHighlight);
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

void AN3DNonogram::GetCurrentSelection(ESelectionType& Type, int& Index) const
{
	Type = CurrentSelection.Key;
	Index = CurrentSelection.Value;
}

bool AN3DNonogram::GetNonogramKey(const FNonogramKey& Key, TArray<int32>& Output) const
{
	if (!NonogramKey.Contains(Key))
	{
		return false;
	}

	Output = NonogramKey[Key];
	return true;
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
	DefaultMaterialOnAllCubes();
	RepositionPawn();
}

bool AN3DNonogram::GetNonogramSolvingElapsedTime(float& SolvingElapsedTime) {
	SolvingElapsedTime = 0.0f;
	if (Mode != EGameMode::Solving)
	{
		return false;
	}

	if (!SolvingStartTime.IsSet())
	{
		return false;
	}

	SolvingElapsedTime = SolvingEndTime.Get(UGameplayStatics::GetTimeSeconds(this)) - SolvingStartTime.GetValue();
	return true;
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

void AN3DNonogram::StoreProgress() const
{
	// Right now, only solving nonogramcan store progress
	if (Mode != EGameMode::Solving)
	{
		return;
	}

	// don't store progress, if solution is finished
	if (CheckSolution())
	{
		return;
	}

	if (GetGameInstance())
	{
		if (UN3DSaveSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<UN3DSaveSubsystem>())
		{
			SaveGameSubsystem->SaveSolvingProgress(CurrentNonogramIndex, SelectedCubes);
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
		GameInstance->OnModeChanged.AddDynamic(this, &ThisClass::OnGameModeChanged);
	}

	// TODO get puzzle info on game start

	/// Temp, setup nonogram based on current puzzle info, move to OnGameModeChanged
	CurrentSize = FIntVector(10);
	SpawnCubeInstances();
	CurrentSelection = { ESelectionType::X, 9 };
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
			// TODO offset the PlaneBase +-48.0f and check against two planes, then check which one is closer to mouse world position
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
					// Start timer in solving mode first time player selects a cube
					if (Mode == EGameMode::Solving && !SolvingStartTime.IsSet())
					{
						SolvingStartTime = UGameplayStatics::GetTimeSeconds(this);
						OnNonogramSolvingStarted.Broadcast();
					}
					const bool bSelected = SelectCube(Cubes[InstanceCoords]);
					OnCubeSelected.Broadcast(Cubes[InstanceCoords], bSelected);
				}
			}
		}
	}
}

void AN3DNonogram::HighlightAllSelectedCubes()
{
	if (Mode != EGameMode::Editor && Mode != EGameMode::Solving)
	{
		return;
	}

	const TArray<float> Deactivated = ColorScheme->Deactivated.Get();
	TArray<float> Selected = ColorScheme->FilledActive.Get();
	for (const TPair<FIntVector, int32>& Cube : Cubes)
	{
		const bool bSelected = (Mode == EGameMode::Editor && CurrentSolution.Contains(Cube.Value)) || (Mode == EGameMode::Solving && SelectedCubes.Contains(Cube.Value));
		if (!bSelected)
		{
			CubeInstances->SetCustomData(Cube.Value, Deactivated);
		}
		else
		{
			if (Mode == EGameMode::Editor || (Mode == EGameMode::Solving && SolvingEndTime.IsSet()))
			{
				Selected[0] = CurrentSolution[Cube.Value].R;
				Selected[1] = CurrentSolution[Cube.Value].G;
				Selected[2] = CurrentSolution[Cube.Value].B;
			}
			CubeInstances->SetCustomData(Cube.Value, Selected);

			FTransform InstanceTransform;
			CubeInstances->GetInstanceTransform(Cube.Value, InstanceTransform);
			InstanceTransform.SetScale3D(FVector(1));
			CubeInstances->UpdateInstanceTransform(Cube.Value, InstanceTransform);
		}
	}
	CubeInstances->MarkRenderStateDirty();
}

void AN3DNonogram::EndHighlight()
{
	if (Mode != EGameMode::Editor && Mode != EGameMode::Solving)
	{
		return;
	}

	DefaultMaterialOnAllCubes();
	const TPair<ESelectionType, int> LastSelection = CurrentSelection;

	const TArray<float> EmptyInactive = ColorScheme->EmptyInactive.Get();
	const TArray<float> FilledInactive = ColorScheme->FilledInactive.Get();
	for (TPair<FIntVector, int32> Cube : Cubes)
	{
		const bool bSelected = (Mode == EGameMode::Editor && CurrentSolution.Contains(Cube.Value)) || (Mode == EGameMode::Solving && SelectedCubes.Contains(Cube.Value));
		CubeInstances->SetCustomData(Cube.Value, bSelected ? FilledInactive : EmptyInactive);

		FTransform InstanceTransform;
		CubeInstances->GetInstanceTransform(Cube.Value, InstanceTransform);
		InstanceTransform.SetScale3D(CubeScale);
		CubeInstances->UpdateInstanceTransform(Cube.Value, InstanceTransform);
	}
	SelectPlane(LastSelection.Key, LastSelection.Value);
}

void AN3DNonogram::OnGameModeChanged(const EGameMode NewMode, const EGameMode PreviousMode)
{
	Mode = NewMode;
	CurrentSolution.Reset();
	SelectedCubes.Reset();
	NonogramKey.Reset();
	SelectPlane(ESelectionType::X, 0);

	switch (Mode)
	{
	case EGameMode::MainMenu:
		DefaultMaterialOnAllCubes();
		break;
	case EGameMode::Solving:		
		if (UN3DGameInstance* GameInstance = Cast<UN3DGameInstance>(UGameplayStatics::GetGameInstance(this)))
		{
			CurrentNonogramIndex = GameInstance->GetSelectedSolution();
			FNonogram Nonogram;
			if (UN3DStatics::GetNonogram(this, CurrentNonogramIndex, Nonogram))
			{
				SetSolution(Nonogram.Nonogram.LoadSynchronous());

				CurrentSize = Nonogram.Size;
				GenerateNonogramKey();

				SpawnCubeInstances();

				if (NonogramKeyWidget)
				{
					NonogramKeyWidget->SetVisibility(true);
				}

				// Load progress if any is saved
				if (UN3DSaveSubsystem* SaveSubsystem = GameInstance->GetSubsystem<UN3DSaveSubsystem>())
				{
					SaveSubsystem->GetSavedProgress(CurrentNonogramIndex, SelectedCubes);
				}

				CurrentSelection = { ESelectionType::X, 0 };
				SelectPlane(CurrentSelection.Key, CurrentSelection.Value);

				SolvingStartTime.Reset();
				SolvingEndTime.Reset();

				if (Controller)
				{
					EnableInput(Controller);
				}

				RepositionPawn();
			}
		}
		break;
	case EGameMode::Editor:
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
	OnSelectionChanged.Broadcast(Selection, Index);
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

void AN3DNonogram::DefaultMaterialOnAllCubes()
{
	const TArray<float> EmptyInactive = ColorScheme->EmptyInactive.Get();
	for (const TPair<FIntVector, int>& Cube : Cubes)
	{
		CubeInstances->SetCustomData(Cube.Value, EmptyInactive);
	}
	SelectPlane(ESelectionType::X, 0);
}

bool AN3DNonogram::SelectCube(const int32 CubeIndex)
{
	bool bSelected = false;

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
			bSelected = true;
		}
		CubeInstances->MarkRenderStateDirty();

		if (CheckSolution())
		{
			FinishSolving();
		}
		break;
	case EGameMode::Editor:
		if (CurrentSolution.Contains(CubeIndex))
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
			UN3DStatics::AddNonogramEditorColor(this, SelectedColor);
			bSelected = true;
		}
		CubeInstances->MarkRenderStateDirty();
		break;
	default:
		break;
	}

	return bSelected;
}

void AN3DNonogram::GenerateNonogramKey()
{
	if (CurrentSolution.IsEmpty())
	{
		return;
	}

	NonogramKey.Empty();

	GenerateNonogramKeyForPlane(ESelectionType::X);
	GenerateNonogramKeyForPlane(ESelectionType::Y);
	GenerateNonogramKeyForPlane(ESelectionType::Z);
}

void AN3DNonogram::GenerateNonogramKeyForPlane(const ESelectionType Plane)
{
	int Axis1;
	int Axis2;
	int Axis3;

	switch (Plane)
	{
	case ESelectionType::X:
		Axis1 = CurrentSize.Y;
		Axis2 = CurrentSize.Z;
		Axis3 = CurrentSize.X;
		break;
	case ESelectionType::Y:
		Axis1 = CurrentSize.X;
		Axis2 = CurrentSize.Z;
		Axis3 = CurrentSize.Y;
		break;
	case ESelectionType::Z:
		Axis1 = CurrentSize.X;
		Axis2 = CurrentSize.Y;
		Axis3 = CurrentSize.Z;
		break;
	default:
		return;
	}

	for (int i = 0; i < Axis1; i++)
	{
		for (int j = 0; j < Axis2; j++)
		{
			FNonogramKey Index(Plane, FIntPoint(i, j));
			if (!NonogramKey.Contains(Index))
			{
				TArray<int> Keys;
				int Counter = 0;
				for (int k = 0; k < Axis3; k++)
				{
					FIntVector CubeIndex;
					switch (Plane)
					{
					case ESelectionType::X:
						CubeIndex = FIntVector(k, i, j);
						break;
					case ESelectionType::Y:
						CubeIndex = FIntVector(i, k, j);
						break;
					case ESelectionType::Z:
						CubeIndex = FIntVector(i, j, k);
						break;
					default:
						return;
					}

					if (CurrentSolution.Contains(Cubes[CubeIndex]))
					{
						Counter++;
					}
					else
					{
						if (Counter > 0)
						{
							Keys.Add(Counter);
							Counter = 0;
						}
					}
				}
				if (Counter > 0)
				{
					Keys.Add(Counter);
				}
				NonogramKey.Add(Index, Keys);
			}
		}
	}
}

void AN3DNonogram::FinishSolving() {
	UE_LOG(LogTemp, Warning, TEXT("Nonogram completed"));
	DisableInput(Controller);
	SolvingEndTime = UGameplayStatics::GetTimeSeconds(this);
	OnNonogramSolvingEnded.Broadcast();
	
	if (NonogramKeyWidget)
	{
		NonogramKeyWidget->SetVisibility(false);
	}
	HighlightAllSelectedCubes();

	if (GetGameInstance())
	{
		if (UN3DSaveSubsystem* SaveGameSubsystem = GetGameInstance()->GetSubsystem<UN3DSaveSubsystem>())
		{
			float CompletitionTime;
			if (GetNonogramSolvingElapsedTime(CompletitionTime))
			{
				SaveGameSubsystem->NonogramSolved(CurrentNonogramIndex, CompletitionTime);
			}
		}
	}
}

void AN3DNonogram::RepositionPawn() {
	if (Mode == EGameMode::Solving || Mode == EGameMode::Editor)
	{
		if (Controller)
		{
			if (APawn* Pawn = Controller->GetPawn())
			{
				const FVector NewPawnPosition = GetActorLocation();
				const FVector Offset = FVector(CurrentSize) * 50.0f;
				Pawn->SetActorLocation(NewPawnPosition + Offset);
			}
		}
	}
}
