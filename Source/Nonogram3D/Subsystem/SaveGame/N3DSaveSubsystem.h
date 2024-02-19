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

	// TODO having save subsystem responsible for both saving/loading and managing available solutions isn't best, rework
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Nonograms")
	TObjectPtr<UN3DNonogramList> Nonograms;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Nonograms")
	TMap<FString, FNonogram> MyCreatedNonograms;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Nonograms")
	TMap<FString, FNonogram> DownloadedNonograms;

	UPROPERTY(BlueprintReadOnly, Category = "SaveGame")
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

	UFUNCTION(BlueprintCallable, Category = "Nonograms")
	bool GetNonogram(const int Index, const ENonogramType Type, FNonogram& Nonogram) const;

	void NonogramSolved(const int Index, const ENonogramType Type, const float SolvingTime);

	TArray<FLinearColor> GetEditorColors() const;
	void AddEditorColor(const FLinearColor& Color);

	/**
	* Stores current solving progress in save file.
	* This DOESN'T save data on disk.
	*/
	void StoreSolvingProgress(const int Index, const ENonogramType Type, const TSet<int32>& SelectedCubes);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void DiscardSolvingProgress();

	/**
	* Discards current editor progress from save file.
	* This DOESN'T save data on disk.
	*/
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void DiscardEditorProgress();

	/**
	* Removes created or dowloaded nonogram from the list.
	* This DOESN'T save data on disk
	*/
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	bool DeleteNonogram(const int Index, const ENonogramType Type);

	/**
	* Stores current editor progress in save file.
	* This DOESN'T save data on disk.
	*/
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void StoreEditorProgress(const FString& NonogramName, const FIntVector& Size, const TMap<int32, FColor> EditorSolution);

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	bool IsNonogramInProgress(const int Index, const ENonogramType Type) const;

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	bool IsAnyNonogramInProgress(int& Index, ENonogramType& Type) const;

	bool GetSavedSolvingProgress(TSet<int32>& SelectedCubes) const;

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	bool IsEditorInProgress() const;

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	bool GetSavedEditorProgress(FString& NonogramName, FIntVector& Size, TMap<int32, FColor>& EditorSolution) const; // TODO when FNonogram will no longer use a data table. Can I use that data structure directly?

	void ResolveCreatedNonograms();

	void ResolveDownloadedNonograms();

	/**
	* Merges nonograms loaded from drive with already known ones
	*/
	void ResolveNonogramsFromDrive(const FString& Directory, TArray<FSavedCreatedNonogramsInfo>& SaveGameInfo, TMap<FString, FNonogram>& LoadedData);

	void ResolveNonogramSolvingProgressForDeletedNonogram(const int Index, const ENonogramType Type);
};
