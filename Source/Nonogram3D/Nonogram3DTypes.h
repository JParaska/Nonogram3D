// Created by Jan Paraska. All rights reserved


#pragma once

// ----------------------------------------------------------------------------------------------------------------
// This header is for enums and structs used by classes and blueprints accross the game
// ----------------------------------------------------------------------------------------------------------------

#include "Nonogram3DTypes.generated.h"

UENUM(BlueprintType)
enum class EGameMode : uint8 {
	MainMenu,		// Game is in main menu. Game just started, nonogram was solved, or level editor saved new file
	Solving,		// Player is solving a nonogram
	Editor			// Player is creating new nonogram in nonogram editor
};

UENUM(BlueprintType)
enum class ESaveLoadError : uint8 {
	Success,
	NotInEditMode,
	EmptySolution,
	InvalidName,
	InvalidPath,
	FailedToSave,
	UnsupportedPlatform
};

USTRUCT(BlueprintType)
struct FNonogramSolution : public FTableRowBase {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CubeIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor FinalColor = FColor::White;
};