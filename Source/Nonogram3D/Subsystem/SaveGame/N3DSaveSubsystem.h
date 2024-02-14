// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Nonogram3DTypes.h"
#include "N3DSaveGameTypes.h"
#include "N3DSaveSubsystem.generated.h"

class UN3DGameInstance;
class UN3DNonogramList;
class UN3DSaveGame;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSaveGameLoaded, const bool, bSuccess);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGameSaved, const bool, bSuccess);

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NONOGRAM3D_API UN3DSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:

	// TODO move this somwhere else
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Nonograms")
	TObjectPtr<UN3DNonogramList> Nonograms;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Nonograms")
	TMap<FString, FNonogram> MyCreatedNonograms;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Nonograms")
	TMap<FString, FNonogram> DownloadedNonograms;

	UPROPERTY(BlueprintReadOnly, Category = "Nonograms")
	TObjectPtr<UN3DSaveGame> SaveGame;
	
public:

	UN3DSaveSubsystem();

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void LoadSavedData(const FOnSaveGameLoaded& Callback);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void SaveGameData(const FOnGameSaved& Callback);

	UFUNCTION(BlueprintPure, Category = "SaveGame")
	FSaveNonogramStatus GetNonogramStatus(const int Index) const;

	// TODO move this somwhere else
	UN3DNonogramList* GetNonograms() const { return Nonograms; }
	TMap<FString, FNonogram> GetMyCreatedNonograms() const { return MyCreatedNonograms; }

	bool IsValidIndex(const int SolutionIndex, const ENonogramType SolutionType) const;
	bool GetNonogram(const int Index, const ENonogramType Type, FNonogram& Nonogram) const;

	void NonogramSolved(const int Index, const ENonogramType Type, const float SolvingTime);

	TArray<FLinearColor> GetEditorColors() const;
	void AddEditorColor(const FLinearColor& Color);

	/**
	* Stores current solving progress in save file.
	* This DOESN'T save data on disk.
	*/
	void StoreSolvingProgress(const int Index, const ENonogramType Type, const TSet<int32>& SelectedCubes);

	/**
	* Discards current editor progress from save file.
	* This DOESN'T save data on disk.
	*/
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void DiscardEditorProgress();

	/**
	* Stores current editor progress in save file.
	* This DOESN'T save data on disk.
	*/
	void StoreEditorProgress(const FIntVector& Size, const TMap<int32, FColor> EditorSolution);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	bool IsAnyNonogramInProgress(int& Index) const;

	bool GetSavedSolvingProgress(const int Index, TSet<int32>& SelectedCubes) const;

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	bool IsEditorInProgress() const;

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	bool GetSavedEditorProgress(FIntVector& Size, TMap<int32, FColor>& EditorSolution) const;

	/**
	* Merges nonograms loaded from drive with already known ones
	*/
	void ResolveCreatedNonograms();

	void ResolveDownloadedNonograms();

	void ResolveNonogramsFromDrive(const FString& Directory, TArray<FSavedCreatedNonogramsInfo>& SaveGameInfo, TMap<FString, FNonogram>& LoadedData);
};
