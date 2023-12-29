// Created by Jan Paraska. All rights reserved


#pragma once

// ----------------------------------------------------------------------------------------------------------------
// This header is for enums and structs used by classes and blueprints accross the game
// ----------------------------------------------------------------------------------------------------------------

#include "Nonogram3DTypes.generated.h"

USTRUCT(BlueprintType)
struct FNonogramSolution : public FTableRowBase {

	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CubeIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor FinalColor = FColor::White;
};