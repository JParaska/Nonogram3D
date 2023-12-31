// Created by Jan Paraska. All rights reserved

#include "N3DGameInstance.h"


void UN3DGameInstance::SetMode(const EGameMode NewMode)
{
	Mode = NewMode;
	OnModeChanged.Broadcast(Mode);
}
