// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "N3DGameInstance.generated.h"

class ANonogram;

DECLARE_DELEGATE_OneParam(FOnNonogramSet, ANonogram*);

UCLASS()
class NONOGRAM3D_API UN3DGameInstance : public UGameInstance
{
	GENERATED_BODY()

#pragma region Properties
public:

	FOnNonogramSet OnNonogramSet;

protected:

	TObjectPtr<ANonogram> Nonogram;
#pragma endregion

#pragma region Methods
public:

	ANonogram* GetActiveNonogram() const { return Nonogram; }

	void SetActiveNonogram(ANonogram* ActiveNonogram);
#pragma endregion
};
