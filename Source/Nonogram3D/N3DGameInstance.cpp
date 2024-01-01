// Created by Jan Paraska. All rights reserved

#include "N3DGameInstance.h"


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
