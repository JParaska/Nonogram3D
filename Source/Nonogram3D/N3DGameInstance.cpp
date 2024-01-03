// Created by Jan Paraska. All rights reserved

#include "N3DGameInstance.h"

#include "N3DNonogram.h"

#include "Kismet/GameplayStatics.h"


void UN3DGameInstance::SetMode(const EGameMode NewMode)
{
	Mode = NewMode;
	if (Mode != EGameMode::Solving)
	{
		SelectedSolution = FNonogram();
	}
	OnModeChanged.Broadcast(Mode);
}

void UN3DGameInstance::StartSolving(const FNonogram& Solution)
{
	if (ensure(Solution.IsValid()))
	{
		SelectedSolution = Solution;
		SetMode(EGameMode::Solving);
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
				for (auto Cube : Solution)
				{
					Nonogram->SelectCube(Cube.Key);
				}
				Nonogram->DeselectAllCubes();
				Nonogram->SelectPlane(ESelectionType::X, 0);
			}
		}
	}
#endif // WITH_EDITOR
}
