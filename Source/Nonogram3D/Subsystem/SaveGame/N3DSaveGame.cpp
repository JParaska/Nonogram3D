// Created by Jan Paraska. All rights reserved


#include "N3DSaveGame.h"

UN3DSaveGame::UN3DSaveGame()
{
	LastUsedEditorColors.Reserve(10);
}

void UN3DSaveGame::AddEditorColor(const FLinearColor& Color)
{
	if (LastUsedEditorColors.Contains(Color))
	{
		return;
	}

	while (LastUsedEditorColors.Num() > 10) // TODO move this to developer settings
	{
		LastUsedEditorColors.RemoveAt(0); // Keep removing old colors to make room for new one
	}

	LastUsedEditorColors.Add(Color);
}
