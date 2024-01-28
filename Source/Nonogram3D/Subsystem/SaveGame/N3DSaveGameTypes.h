// Created by Jan Paraska. All rights reserved


#pragma once

// ----------------------------------------------------------------------------------------------------------------
// This header is for enums and structs used by classes and blueprints accross the game
// ----------------------------------------------------------------------------------------------------------------

#include "N3DSaveGameTypes.generated.h"

UENUM(BlueprintType)
enum class ENonogramStatus : uint8
{
	Locked,
	Unlocked,
	InProgress,
	Completed
};

USTRUCT(BlueprintType)
struct FSaveNonogramStatus {

	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ENonogramStatus Status = ENonogramStatus::Locked;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CompletitionTime = TNumericLimits<float>::Max();
};

USTRUCT(BlueprintType)
struct FSavedSolvingProgress {

	GENERATED_BODY()

public:

	FSavedSolvingProgress() {}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int Index = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSet<int32> SelectedCubes;

	bool IsSet() const { return Index >= 0 && !SelectedCubes.IsEmpty(); }

	void Reset() {
		Index = -1;
		SelectedCubes.Reset();
	}
};
