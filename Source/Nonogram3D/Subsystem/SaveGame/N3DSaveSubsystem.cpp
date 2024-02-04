// Created by Jan Paraska. All rights reserved


#include "N3DSaveSubsystem.h"

#include "N3DNonogramList.h"
#include "N3DSaveGame.h"

#include "Kismet/GameplayStatics.h"

#define SLOT_NAME "SaveGame"
#define SLOT_INDEX 0

void UN3DSaveSubsystem::LoadSavedData(const FOnSaveGameLoaded& Callback)
{
	UGameplayStatics::AsyncLoadGameFromSlot(SLOT_NAME, SLOT_INDEX, FAsyncLoadGameFromSlotDelegate::CreateLambda([&, Callback](const FString& SlotName, const int32 Index, USaveGame* LoadedGame) {
		if (SaveGame = Cast<UN3DSaveGame>(LoadedGame))
		{
			UE_LOG(LogTemp, Warning, TEXT("Save game loaded"));
			Callback.ExecuteIfBound(true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Save game doesn't exist, creating..."));
			if (!Nonograms || Nonograms->Nonograms.IsEmpty())
			{
				Callback.ExecuteIfBound(false);
			}

			SaveGame = NewObject<UN3DSaveGame>(this);
			SaveGame->Nonograms;
			SaveGame->Nonograms.Reserve(Nonograms->Nonograms.Num());
			for (int i = 0; i < Nonograms->Nonograms.Num(); i++)
			{
				SaveGame->Nonograms.Add(FSaveNonogramStatus());
			}
			SaveGame->Nonograms[0].Status = ENonogramStatus::Unlocked;

			UGameplayStatics::AsyncSaveGameToSlot(SaveGame, SLOT_NAME, SLOT_INDEX, FAsyncSaveGameToSlotDelegate::CreateLambda([&, Callback](const FString& SlotName, const int32 Index, bool bSuccess) {
				UE_LOG(LogTemp, Warning, TEXT("Save game created"));
				Callback.ExecuteIfBound(bSuccess);
			}));
		}
		}));
}

void UN3DSaveSubsystem::SaveGameData(const FOnGameSaved& Callback)
{
	UGameplayStatics::AsyncSaveGameToSlot(SaveGame, SLOT_NAME, SLOT_INDEX, FAsyncSaveGameToSlotDelegate::CreateLambda([&, Callback](const FString& SlotName, const int32 Index, bool bSuccess) {
		if (bSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("Game saved"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Game failed to save"));
		}
		Callback.ExecuteIfBound(bSuccess);
	}));
}

FSaveNonogramStatus UN3DSaveSubsystem::GetNonogramStatus(const int Index) const
{
	if (!ensure(SaveGame))
	{
		return FSaveNonogramStatus();
	}
	if (!ensure(SaveGame->Nonograms.IsValidIndex(Index)))
	{
		return FSaveNonogramStatus();
	}

	return SaveGame->Nonograms[Index];
}

void UN3DSaveSubsystem::NonogramSolved(const int Index, const float CompletitionTime)
{
	if (!SaveGame || !SaveGame->Nonograms.IsValidIndex(Index))
	{
		return;
	}

	FSaveNonogramStatus& SolvedNonogramState = SaveGame->Nonograms[Index];
	SolvedNonogramState.Status = ENonogramStatus::Completed;
	
	// If new completition time is faster, update it
	if (SolvedNonogramState.CompletitionTime > CompletitionTime)
	{
		SolvedNonogramState.CompletitionTime = CompletitionTime;
	}
	
	// If next nonogram is locked, unlock it
	if (SaveGame->Nonograms.IsValidIndex(Index + 1))
	{
		if (SaveGame->Nonograms[Index + 1].Status == ENonogramStatus::Locked)
		{
			SaveGame->Nonograms[Index + 1].Status = ENonogramStatus::Unlocked;
		}
	}

	// If finished nonogram had saved progress, reset progress
	if (SaveGame->SolvingProgress.IsSet() && SaveGame->SolvingProgress.Index == Index)
	{
		SaveGame->SolvingProgress.Reset();
	}
}

TArray<FLinearColor> UN3DSaveSubsystem::GetEditorColors() const
{
	return SaveGame ? SaveGame->LastUsedEditorColors : TArray<FLinearColor>();
}

void UN3DSaveSubsystem::AddEditorColor(const FLinearColor& Color)
{
	if (SaveGame)
	{
		SaveGame->AddEditorColor(Color);
	}
}

void UN3DSaveSubsystem::StoreSolvingProgress(const int Index, const TSet<int32>& SelectedCubes)
{
	if (SaveGame && SaveGame->Nonograms.IsValidIndex(Index))
	{
		SaveGame->SolvingProgress.Index = Index;
		SaveGame->SolvingProgress.SelectedCubes.Append(SelectedCubes);

		if (SaveGame->Nonograms[Index].Status == ENonogramStatus::Unlocked)
		{
			SaveGame->Nonograms[Index].Status = ENonogramStatus::InProgress;
		}
	}
}

void UN3DSaveSubsystem::DiscardEditorProgress()
{
	if (SaveGame)
	{
		SaveGame->EditorProgress.Reset();
	}
}

void UN3DSaveSubsystem::StoreEditorProgress(const FIntVector& Size, const TMap<int32, FColor> EditorSolution)
{
	if (SaveGame)
	{
		SaveGame->EditorProgress.Reset();

		FSavedEditorProgress EditorProgress;
		EditorProgress.Size = Size;
		EditorProgress.Solution = EditorSolution;

		SaveGame->EditorProgress = EditorProgress;
	}
}

bool UN3DSaveSubsystem::IsAnyNonogramInProgress(int& Index) const
{
	if (SaveGame && SaveGame->SolvingProgress.IsSet())
	{
		Index = SaveGame->SolvingProgress.Index;
		return true;
	}
	return false;
}

bool UN3DSaveSubsystem::GetSavedSolvingProgress(const int Index, TSet<int32>& SelectedCubes) const
{
	if (SaveGame && SaveGame->SolvingProgress.IsSet() && SaveGame->SolvingProgress.Index == Index)
	{
		SelectedCubes.Reset();
		SelectedCubes.Append(SaveGame->SolvingProgress.SelectedCubes);
		return true;
	}

	return false;
}

bool UN3DSaveSubsystem::IsEditorInProgress() const
{
	return SaveGame && SaveGame->EditorProgress.IsSet();
}

bool UN3DSaveSubsystem::GetSavedEditorProgress(FIntVector& Size, TMap<int32, FColor>& EditorSolution) const
{
	if (SaveGame && SaveGame->EditorProgress.IsSet())
	{
		Size = SaveGame->EditorProgress.Size;

		EditorSolution.Reset();
		EditorSolution.Append(SaveGame->EditorProgress.Solution);

		return true;
	}

	return false;
}
