// Created by Jan Paraska. All rights reserved


#pragma once

// ----------------------------------------------------------------------------------------------------------------
// This header is for enums and structs used by classes and blueprints accross the game
// ----------------------------------------------------------------------------------------------------------------

#include "Nonogram3DTypes.generated.h"

class UDataTable;

UENUM(BlueprintType)
enum class ESelectionType : uint8
{
	X,
	Y,
	Z
};

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

USTRUCT(BlueprintType)
struct FNonogram {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText NonogramName = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector Size = FIntVector(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UDataTable> Nonogram = nullptr;

	bool IsValid() const {
		return !NonogramName.IsEmpty() && !Size.IsZero(); // TODO test also if Nonogram is valid (either loaded or pointing to valid object)
	}
};

USTRUCT(BlueprintType)
struct FNonogramKey {

	// X is row for Y plane and column for Z plane
	// Y is row for X and Z plane
	// Z is column for X and Y plane

	GENERATED_BODY()

public:

	FNonogramKey() {};

	FNonogramKey(ESelectionType Selection, FIntPoint Index) : Selection(Selection), Index(Index) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESelectionType Selection;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint Index;

	bool operator==(const FNonogramKey& Other) const {
		return Selection == Other.Selection && Index == Other.Index;
	}
};

FORCEINLINE uint32 GetTypeHash(const FNonogramKey& NonogramKey)
{
	uint32 Hash = FCrc::MemCrc32(&NonogramKey, sizeof(FNonogramKey));
	return Hash;
}