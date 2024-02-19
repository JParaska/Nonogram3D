// Created by Jan Paraska. All rights reserved


#pragma once

// ----------------------------------------------------------------------------------------------------------------
// This header is for enums and structs used by classes and blueprints accross the game
// ----------------------------------------------------------------------------------------------------------------

#include "Nonogram3DTypes.h"

#include "N3DSaveGameTypes.generated.h"

UENUM(BlueprintType)
enum class ENonogramStatus : uint8
{
	Locked,
	Unlocked,
	InProgress, // TODO I don't think I still need this
	Completed
};

USTRUCT(BlueprintType)
struct FSaveNonogramStatus
{

	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ENonogramStatus Status = ENonogramStatus::Locked;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CompletitionTime = TNumericLimits<float>::Max();
};

USTRUCT(BlueprintType)
struct FSavedCreatedNonogramsInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString CreatedNonogramName;

	// TODO save also size, and check it against loaded data
};

USTRUCT(BlueprintType)
struct FSavedSolvingProgress
{

	GENERATED_BODY()

public:

	FSavedSolvingProgress() {}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int Index = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ENonogramType Type;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSet<int32> SelectedCubes;

	bool IsSet() const { return Index >= 0 && !SelectedCubes.IsEmpty(); }
	
	void Reset()
	{
		Index = -1;
		SelectedCubes.Reset();
	}
};

USTRUCT(BlueprintType)
struct FSavedEditorProgress
{

	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString NonogramName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FIntVector Size = FIntVector::NoneValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<int32, FColor> Solution;

	bool IsSet() const { return Size != FIntVector::NoneValue && !Solution.IsEmpty(); }

	void Reset()
	{
		Size = FIntVector::NoneValue;
		Solution.Reset();
	}
};
