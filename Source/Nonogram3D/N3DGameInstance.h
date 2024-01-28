// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Nonogram3DTypes.h"
#include "N3DGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnModeChanged, const EGameMode, NewMode, const EGameMode, PreviousMode);

UCLASS()
class NONOGRAM3D_API UN3DGameInstance : public UGameInstance
{
	GENERATED_BODY()

#pragma region Properties
public:

	UPROPERTY(BlueprintAssignable)
	FOnModeChanged OnModeChanged;

protected:

	EGameMode Mode = EGameMode::MainMenu;

	UPROPERTY()
	int SelectedSolution = 0;
#pragma endregion

#pragma region Methods
public:

	UFUNCTION(BlueprintPure)
	EGameMode GetMode() const { return Mode; }

	int GetSelectedSolution() const { return SelectedSolution; }

	UFUNCTION(BlueprintCallable)
	void SetMode(const EGameMode NewMode);

	UFUNCTION(BlueprintCallable)
	void StartSolving(const int SolutionIndex);

	/**
	* Solves current nonogram. Applies only in solving mode
	*/
	UFUNCTION(Exec)
	void Solve();
#pragma endregion
};
