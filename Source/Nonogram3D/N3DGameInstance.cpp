// Created by Jan Paraska. All rights reserved

#include "N3DGameInstance.h"

void UN3DGameInstance::SetActiveNonogram(ANonogram* ActiveNonogram)
{
	Nonogram = ActiveNonogram;
	OnNonogramSet.ExecuteIfBound(Nonogram);
}
