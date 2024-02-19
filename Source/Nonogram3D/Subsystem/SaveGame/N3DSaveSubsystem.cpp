// Created by Jan Paraska. All rights reserved


#include "N3DSaveSubsystem.h"

#include "N3DNonogramList.h"
#include "N3DSaveGame.h"
#include "N3DStatics.h"

#include "GeneralProjectSettings.h"
#include "Kismet/GameplayStatics.h"

#define SLOT_NAME "SaveGame"
#define SLOT_INDEX 0

UN3DSaveSubsystem::UN3DSaveSubsystem()
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString Path;

#if PLATFORM_WINDOWS
	if (const UGeneralProjectSettings* ProjectSettings = GetDefault<UGeneralProjectSettings>())
	{
		Path = FPaths::Combine(FPlatformProcess::UserDir(), ProjectSettings->ProjectName);
	}
	else
	{
		Path = FPaths::Combine(FPlatformProcess::UserDir(), DEFAULT_PROJECT_NAME);
	}
#else
#error "Unsupported platform!"
#endif

	FString CreatedPath = FPaths::Combine(Path, CREATED_NONOGRAMS);
	if (!PlatformFile.DirectoryExists(*CreatedPath))
	{
		PlatformFile.CreateDirectory(*CreatedPath);
	}

	FString DownloadedPath = FPaths::Combine(Path, DOWNLOADED_NONOGRAMS);
	if (!PlatformFile.DirectoryExists(*DownloadedPath))
	{
		PlatformFile.CreateDirectory(*DownloadedPath);
	}
}

void UN3DSaveSubsystem::LoadSavedData(const FOnSaveGameLoaded& Callback)
{
	UGameplayStatics::AsyncLoadGameFromSlot(SLOT_NAME, SLOT_INDEX, FAsyncLoadGameFromSlotDelegate::CreateLambda([&, Callback](const FString& SlotName, const int32 Index, USaveGame* LoadedGame) {
		if (SaveGame = Cast<UN3DSaveGame>(LoadedGame))
		{
			UE_LOG(LogTemp, Warning, TEXT("Save game loaded"));
			ResolveCreatedNonograms();
			ResolveDownloadedNonograms();

			// Re-save data as created/downloaded nonograms can alter it
			UGameplayStatics::AsyncSaveGameToSlot(SaveGame, SLOT_NAME, SLOT_INDEX, FAsyncSaveGameToSlotDelegate::CreateLambda([&, Callback](const FString& SlotName, const int32 Index, bool bSuccess) {
				UE_LOG(LogTemp, Warning, TEXT("Save game re-saved after loading created/downloaded nonograms"));
				Callback.ExecuteIfBound(bSuccess);
			}));
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

			ResolveCreatedNonograms();
			ResolveDownloadedNonograms();

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

bool UN3DSaveSubsystem::IsValidIndex(const int SolutionIndex, const ENonogramType SolutionType) const
{
	switch (SolutionType)
	{
	case ENonogramType::Default:
		return Nonograms->Nonograms.IsValidIndex(SolutionIndex);
	case ENonogramType::Created:
		return SaveGame && SaveGame->CreatedNonogramsInfo.IsValidIndex(SolutionIndex) && MyCreatedNonograms.Contains(SaveGame->CreatedNonogramsInfo[SolutionIndex].CreatedNonogramName);
	case ENonogramType::Downloaded:
		return SaveGame && SaveGame->DownloadedNonogramsInfo.IsValidIndex(SolutionIndex) && DownloadedNonograms.Contains(SaveGame->DownloadedNonogramsInfo[SolutionIndex].CreatedNonogramName);
	default:
		return false;
	}
}

bool UN3DSaveSubsystem::GetNonogram(const int Index, const ENonogramType Type, FNonogram& Nonogram) const
{
	if (!IsValidIndex(Index, Type))
	{
		return false;
	}

	switch (Type)
	{
	case ENonogramType::Default:
		Nonogram = Nonograms->Nonograms[Index];
		break;
	case ENonogramType::Created:
		Nonogram = MyCreatedNonograms[SaveGame->CreatedNonogramsInfo[Index].CreatedNonogramName];
		break;
	case ENonogramType::Downloaded:
		Nonogram = DownloadedNonograms[SaveGame->DownloadedNonogramsInfo[Index].CreatedNonogramName];
	default:
		return false;
	}

	return true;
}

void UN3DSaveSubsystem::NonogramSolved(const int Index, const ENonogramType Type, const float CompletitionTime)
{
	if (!SaveGame || !IsValidIndex(Index, Type))
	{
		return;
	}

	// If solving one of default nonograms, mark it as solved and unlock next one
	if (Type == ENonogramType::Default)
	{
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
	}

	// If finished nonogram had saved progress, reset progress
	if (SaveGame->SolvingProgress.IsSet() && SaveGame->SolvingProgress.Index == Index && SaveGame->SolvingProgress.Type == Type)
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

void UN3DSaveSubsystem::StoreSolvingProgress(const int Index, const ENonogramType Type, const TSet<int32>& SelectedCubes)
{
	if (SaveGame && IsValidIndex(Index, Type))
	{
		SaveGame->SolvingProgress.Index = Index;
		SaveGame->SolvingProgress.Type = Type;

		SaveGame->SolvingProgress.SelectedCubes.Reset();
		SaveGame->SolvingProgress.SelectedCubes.Append(SelectedCubes);

		if (Type == ENonogramType::Default && SaveGame->Nonograms[Index].Status == ENonogramStatus::Unlocked)
		{
			SaveGame->Nonograms[Index].Status = ENonogramStatus::InProgress;
		}
	}
}

void UN3DSaveSubsystem::DiscardSolvingProgress()
{
	if (SaveGame && SaveGame->SolvingProgress.IsSet())
	{
		SaveGame->SolvingProgress.Reset();
	}
}

void UN3DSaveSubsystem::DiscardEditorProgress()
{
	if (SaveGame)
	{
		SaveGame->EditorProgress.Reset();
	}
}

bool UN3DSaveSubsystem::DeleteNonogram(const int Index, const ENonogramType Type)
{
	if (!SaveGame)
	{
		return false;
	}

	if (Type != ENonogramType::Created && Type != ENonogramType::Downloaded)
	{
		return false;
	}

	bool bDeleted = false;

	switch (Type)
	{
	case ENonogramType::Created:
		if (SaveGame->CreatedNonogramsInfo.IsValidIndex(Index))
		{
			FString Name = SaveGame->CreatedNonogramsInfo[Index].CreatedNonogramName;
			SaveGame->CreatedNonogramsInfo.RemoveAt(Index);
			MyCreatedNonograms.Remove(Name);
			UN3DStatics::DeleteNonogram(Name, CREATED_NONOGRAMS);
			ResolveNonogramSolvingProgressForDeletedNonogram(Index, Type);
			bDeleted = true;
		}
		break;
	case ENonogramType::Downloaded:
		if (SaveGame->DownloadedNonogramsInfo.IsValidIndex(Index))
		{
			FString Name = SaveGame->DownloadedNonogramsInfo[Index].CreatedNonogramName;
			SaveGame->DownloadedNonogramsInfo.RemoveAt(Index);
			DownloadedNonograms.Remove(Name);
			UN3DStatics::DeleteNonogram(Name, DOWNLOADED_NONOGRAMS);
			ResolveNonogramSolvingProgressForDeletedNonogram(Index, Type);
			bDeleted = true;
		}
		break;
	default:
		break;
	}

	return bDeleted;
}

void UN3DSaveSubsystem::StoreEditorProgress(const FString& NonogramName, const FIntVector& Size, const TMap<int32, FColor> EditorSolution)
{
	if (SaveGame)
	{
		SaveGame->EditorProgress.Reset();

		FSavedEditorProgress EditorProgress;
		EditorProgress.NonogramName = NonogramName;
		EditorProgress.Size = Size;
		EditorProgress.Solution = EditorSolution;

		SaveGame->EditorProgress = EditorProgress;
	}
}

bool UN3DSaveSubsystem::IsNonogramInProgress(const int Index, const ENonogramType Type) const
{
	if (SaveGame && SaveGame->SolvingProgress.IsSet())
	{
		return SaveGame->SolvingProgress.Index == Index && SaveGame->SolvingProgress.Type == Type;
	}
	return false;
}

bool UN3DSaveSubsystem::IsAnyNonogramInProgress(int& Index, ENonogramType& Type) const
{
	if (SaveGame && SaveGame->SolvingProgress.IsSet())
	{
		Index = SaveGame->SolvingProgress.Index;
		Type = SaveGame->SolvingProgress.Type;
		return true;
	}
	return false;
}

bool UN3DSaveSubsystem::GetSavedSolvingProgress(TSet<int32>& SelectedCubes) const
{
	if (SaveGame && SaveGame->SolvingProgress.IsSet())
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

bool UN3DSaveSubsystem::GetSavedEditorProgress(FString& NonogramName, FIntVector& Size, TMap<int32, FColor>& EditorSolution) const
{
	if (SaveGame && SaveGame->EditorProgress.IsSet())
	{
		NonogramName = SaveGame->EditorProgress.NonogramName;
		Size = SaveGame->EditorProgress.Size;

		EditorSolution.Reset();
		EditorSolution.Append(SaveGame->EditorProgress.Solution);

		return true;
	}

	return false;
}

void UN3DSaveSubsystem::ResolveCreatedNonograms()
{
	if (!SaveGame)
	{
		return;
	}

	ResolveNonogramsFromDrive(CREATED_NONOGRAMS, SaveGame->CreatedNonogramsInfo, MyCreatedNonograms);
}

void UN3DSaveSubsystem::ResolveDownloadedNonograms()
{
	if (!SaveGame)
	{
		return;
	}

	ResolveNonogramsFromDrive(DOWNLOADED_NONOGRAMS, SaveGame->DownloadedNonogramsInfo, DownloadedNonograms);
}

void UN3DSaveSubsystem::ResolveNonogramsFromDrive(const FString& Directory, TArray<FSavedCreatedNonogramsInfo>& SaveGameInfo, TMap<FString, FNonogram>& LoadedData)
{
	if (!SaveGame)
	{
		return;
	}

	ENonogramType LoadedType;
	if (Directory == CREATED_NONOGRAMS)
	{
		LoadedType = ENonogramType::Created;
	}
	else if (Directory == DOWNLOADED_NONOGRAMS)
	{
		LoadedType = ENonogramType::Downloaded;
	}
	else
	{
		return;
	}

	// Load already known nonogram info with empty solutions
	if (!SaveGameInfo.IsEmpty())
	{
		LoadedData.Reserve(SaveGameInfo.Num());
		for (const FSavedCreatedNonogramsInfo& SavedNonogramInfo : SaveGameInfo)
		{
			LoadedData.Add(SavedNonogramInfo.CreatedNonogramName, FNonogram());
		}
	}

	// Read nonogram solutions from drive
	TArray<FNonogram> LoadedNonograms;
	UN3DStatics::FindNonogramsOnDrive(Directory, LoadedNonograms);

	LoadedData.Reserve(LoadedData.Num() + LoadedNonograms.Num());
	for (const FNonogram& LoadedNonogram : LoadedNonograms)
	{
		if (!LoadedData.Contains(LoadedNonogram.NonogramName))
		{
			// If this is a new nonogram, add it to lists
			LoadedData.Add(LoadedNonogram.NonogramName, LoadedNonogram);

			FSavedCreatedNonogramsInfo SavedInfo;
			SavedInfo.CreatedNonogramName = LoadedNonogram.NonogramName;
			SaveGameInfo.Add(SavedInfo);
		}
		else
		{
			// Otherwise, fill solution
			LoadedData[LoadedNonogram.NonogramName] = LoadedNonogram;
		}
	}

	// Remove nonograms manually removed by player from drive
	if (!SaveGameInfo.IsEmpty())
	{
		for (int i = SaveGameInfo.Num() - 1; i >= 0; i--)
		{
			const FString RemovedNonogramName = SaveGameInfo[i].CreatedNonogramName;
			if (!LoadedData.Contains(RemovedNonogramName))
			{
				ResolveNonogramSolvingProgressForDeletedNonogram(i, LoadedType);

				SaveGameInfo.RemoveAt(i);
			}
			else if (!LoadedData[RemovedNonogramName].IsValid())
			{
				ResolveNonogramSolvingProgressForDeletedNonogram(i, LoadedType);

				SaveGameInfo.RemoveAt(i);
				LoadedData.Remove(RemovedNonogramName);
			}
		}
	}
}

void UN3DSaveSubsystem::ResolveNonogramSolvingProgressForDeletedNonogram(const int Index, const ENonogramType Type)
{
	if (SaveGame->SolvingProgress.IsSet() && SaveGame->SolvingProgress.Type == Type)
	{
		// If deleted nonogram was in progress, remove progress from save game
		if (SaveGame->SolvingProgress.Index == Index)
		{
			SaveGame->SolvingProgress.Reset();
		}
		// If nonogram in progress had highier index, then deleted nonogram, decrease index, to keep it synched
		else if (SaveGame->SolvingProgress.Index > Index)
		{
			SaveGame->SolvingProgress.Index--;
		}
	}
}
