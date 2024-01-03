// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Nonogram3DTypes.h"
#include "N3DGameInstance.generated.h"

class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModeChanged, const EGameMode, NewMode);

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
	FNonogram SelectedSolution = FNonogram();
#pragma endregion

#pragma region Methods
public:

	UFUNCTION(BlueprintPure)
	EGameMode GetMode() const { return Mode; }

	FNonogram GetSelectedSolution() const { return SelectedSolution; }

	UFUNCTION(BlueprintCallable)
	void SetMode(const EGameMode NewMode);

	UFUNCTION(BlueprintCallable)
	void StartSolving(const FNonogram& Solution);

	UFUNCTION(Exec)
	void Solve();
#pragma endregion
};
