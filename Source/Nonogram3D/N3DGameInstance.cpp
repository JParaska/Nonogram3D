// Created by Jan Paraska. All rights reserved

#include "N3DGameInstance.h"

void UN3DGameInstance::SetActiveNonogram(AN3DNonogram* ActiveNonogram)
{
	Nonogram = ActiveNonogram;
	OnNonogramSet.ExecuteIfBound(Nonogram);
}
