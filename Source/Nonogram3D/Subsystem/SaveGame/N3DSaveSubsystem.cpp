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
	if (!SaveGame || SaveGame->Nonograms.IsValidIndex(Index))
	{
		OnGameSaved.Broadcast(false);
	}

	FSaveNonogramStatus& SolvedNonogramState = SaveGame->Nonograms[Index];
	SolvedNonogramState.Status = ENonogramStatus::Completed;
	SolvedNonogramState.CompletitionTime = CompletitionTime;
	if (SaveGame->Nonograms.IsValidIndex(Index + 1))
	{
		SaveGame->Nonograms[Index + 1].Status = ENonogramStatus::Unlocked;
	}

	UGameplayStatics::AsyncSaveGameToSlot(SaveGame, SLOT_NAME, SLOT_INDEX, FAsyncSaveGameToSlotDelegate::CreateLambda([&](const FString& SlotName, const int32 Index, bool bSuccess) {
		UE_LOG(LogTemp, Warning, TEXT("Game saved"));
		OnGameSaved.Broadcast(bSuccess);
	}));
}
