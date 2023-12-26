// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "N3DGameInstance.generated.h"

class AN3DNonogram;

DECLARE_DELEGATE_OneParam(FOnNonogramSet, AN3DNonogram*);

UCLASS()
class NONOGRAM3D_API UN3DGameInstance : public UGameInstance
{
	GENERATED_BODY()

#pragma region Properties
public:

	FOnNonogramSet OnNonogramSet;

protected:

	TObjectPtr<AN3DNonogram> Nonogram;
#pragma endregion

#pragma region Methods
public:

	AN3DNonogram* GetActiveNonogram() const { return Nonogram; }

	void SetActiveNonogram(AN3DNonogram* ActiveNonogram);
#pragma endregion
};
