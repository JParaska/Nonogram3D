// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Nonogram3DTypes.h"
#include "N3DNonogram.generated.h"

class UN3DNonogramColorScheme;
class UN3DNonogramInput;

class APlayerController;
class UDataTable;
class UInstancedStaticMeshComponent;
class UWidgetComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSelectionChanged, const ESelectionType, Type, const int32, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNonogramSolvingStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNonogramSolvingEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCubeSelected, const int32, CubeIndex, const bool, bSelected);

USTRUCT(BlueprintType)
struct FSelectionPlane
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TSet<int> Plane;
};

USTRUCT(BlueprintType)
struct FSelectionCollection
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TMap<int, FSelectionPlane> Collection;
};

UCLASS()
class NONOGRAM3D_API AN3DNonogram : public AActor
{
	GENERATED_BODY()

#pragma region Properties
public:

	UPROPERTY(BlueprintAssignable)
	FOnSelectionChanged OnSelectionChanged;

	UPROPERTY(BlueprintAssignable)
	FOnNonogramSolvingStarted OnNonogramSolvingStarted;

	UPROPERTY(BlueprintAssignable)
	FOnNonogramSolvingEnded OnNonogramSolvingEnded;

	UPROPERTY(BlueprintAssignable)
	FOnCubeSelected OnCubeSelected;

protected:

	UPROPERTY(VisibleAnywhere, Category = "Scene")
	TObjectPtr<UInstancedStaticMeshComponent> CubeInstances;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scene")
	TObjectPtr<UWidgetComponent> NonogramKeyWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	FVector CubeScale = FVector(0.99f);

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TObjectPtr<UN3DNonogramColorScheme> ColorScheme;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UN3DNonogramInput> NonogramInput;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Nonogram")
	FIntVector CurrentSize;

	UPROPERTY(VisibleAnywhere)
	TMap<FIntVector, int> Cubes;

	UPROPERTY(VisibleAnywhere)
	TMap<ESelectionType, FSelectionCollection> SelectionCollection;

	TPair<ESelectionType, int> CurrentSelection;

	TObjectPtr<APlayerController> Controller;

	TMap<int32, FColor> CurrentSolution;

	int CurrentNonogramIndex;

	UPROPERTY(BlueprintReadOnly, Category = "Nonogram")
	TSet<int32> SelectedCubes;

	TMap<FNonogramKey, TArray<int32>> NonogramKey;

#pragma region NonogramEditor

	FColor SelectedColor = FColor::White;
#pragma endregion

#pragma region NonogramSoving

	TOptional<float> SolvingStartTime;
	TOptional<float> SolvingEndTime;
#pragma endregion

private:

	EGameMode Mode = EGameMode::MainMenu;

#pragma endregion
	
#pragma region Methods
public:	
	
	AN3DNonogram();

	virtual void EnableInput(APlayerController* PlayerController) override;

	void SpawnCubeInstances();

	UFUNCTION(BlueprintPure, Category = "Nonogram")
	void GetCurrentSelection(ESelectionType& Type, int& Index) const;

	UFUNCTION(BlueprintPure, Category = "Nonogram")
	TMap<int32, FColor> GetCurrentSolution() const { return CurrentSolution; }

	UFUNCTION(BlueprintPure, Category = "Nonogram")
	bool GetNonogramKey(const FNonogramKey& Key, TArray<int32>& Output) const;

#pragma region NonogramEditor

	UFUNCTION(BlueprintPure, Category = "Nonogram|Nonogram Editor")
	FColor GetSelectedColor() const { return SelectedColor; }

	UFUNCTION(BlueprintCallable, Category = "Nonogram|Nonogram Editor")
	void SetSelectedColor(const FColor& NewColor);

	UFUNCTION(BlueprintCallable, Category = "Nonogram|Nonogram Editor")
	void Resize(const FIntVector& Size);
#pragma endregion

#pragma region NonogramSolving

	UFUNCTION(BlueprintPure, Category = "Nonogram|Nonogram Solving")
	bool GetNonogramSolvingElapsedTime(float& solvingElapsedTime);

	/** Returns true if all (and only) cubes in solution are selected */
	UFUNCTION(BlueprintPure, Category = "Nonogram|Nonogram Solving")
	bool CheckSolution() const;

	UFUNCTION(BlueprintCallable, Category = "Nonogram|Nonogram Solving")
	void StoreProgress() const;
#pragma endregion

protected:

	friend class UN3DGameInstance;
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void SelectionX(const FInputActionValue& Input);

	UFUNCTION()
	void SelectionY(const FInputActionValue& Input);

	UFUNCTION()
	void SelectionZ(const FInputActionValue& Input);

	UFUNCTION()
	void SelectCube();

	UFUNCTION()
	void HighlightAllSelectedCubes();

	UFUNCTION()
	void EndHighlight();

	UFUNCTION()
	void OnGameModeChanged(const EGameMode NewMode, const EGameMode PreviousMode);

	void ResetSelectionCollection(const FIntVector& Size);

	FSelectionCollection CreateSelectionCollection(const int Size);

	void AddInstanceToCollection(const FIntVector& Index, const int InstanceIndex);

	void SelectNext(const ESelectionType Selection, const bool bNext);

	void SelectPlane(const ESelectionType Selection, const int Index);

	void SetSolution(UDataTable* SolutionDataTable);

	void DefaultMaterialOnAllCubes();

	bool SelectCube(const int32 CubeIndex);

	void GenerateNonogramKey();

	void GenerateNonogramKeyForPlane(const ESelectionType Plane);

	void FinishSolving();

	void RepositionPawn();
#pragma endregion
};
