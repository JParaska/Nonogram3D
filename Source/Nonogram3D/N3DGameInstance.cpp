// Created by Jan Paraska. All rights reserved

#include "N3DGameInstance.h"

#include "N3DNonogram.h"
#include "N3DNonogramColorScheme.h"
#include "N3DNonogramList.h"
#include "N3DSaveSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Components/InstancedStaticMeshComponent.h"


void UN3DGameInstance::SetMode(const EGameMode NewMode)
{
	const EGameMode PreviousMode = Mode;
	Mode = NewMode;

	if (Mode != EGameMode::Solving)
	{
		SelectedSolution = -1;
	}
	
	OnModeChanged.Broadcast(Mode, PreviousMode);
}

void UN3DGameInstance::StartSolving(const int SolutionIndex)
{
	if (UN3DSaveSubsystem* SaveSubsystem = GetSubsystem<UN3DSaveSubsystem>())
	{
		if (SaveSubsystem->GetNonograms()->Nonograms.IsValidIndex(SolutionIndex))
		{
			SelectedSolution = SolutionIndex;
			SetMode(EGameMode::Solving);
		}
	}
}

void UN3DGameInstance::Solve()
{
#if WITH_EDITOR
	if (Mode == EGameMode::Solving)
	{
		if (AN3DNonogram* Nonogram = Cast<AN3DNonogram>(UGameplayStatics::GetActorOfClass(this, AN3DNonogram::StaticClass())))
		{
			TMap<int32, FColor> Solution = Nonogram->GetCurrentSolution();
			if (!Solution.IsEmpty())
			{
				Nonogram->SelectedCubes.Empty();
				for (auto Cube : Solution)
				{
					Nonogram->SelectCube(Cube.Key);
				}

				const TArray<float> EmptyInactive = Nonogram->ColorScheme->EmptyInactive.Get();
				const TArray<float> FilledInactive = Nonogram->ColorScheme->FilledInactive.Get();
				for (TPair<FIntVector, int32> Cube : Nonogram->Cubes)
				{
					const bool bSelected = Nonogram->SelectedCubes.Contains(Cube.Value);
					Nonogram->CubeInstances->SetCustomData(Cube.Value, bSelected ? FilledInactive : EmptyInactive);
				}
				Nonogram->FinishSolving();
			}
		}
	}
#endif // WITH_EDITOR
}
