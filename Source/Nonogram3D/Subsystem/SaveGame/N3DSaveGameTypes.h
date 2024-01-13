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