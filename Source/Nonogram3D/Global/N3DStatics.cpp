// Created by Jan Paraska. All rights reserved


#include "N3DStatics.h"

#include "N3DGameInstance.h"
#include "N3DNonogramList.h"
#include "N3DSaveSubsystem.h"

#include "GeneralProjectSettings.h"
#include "Kismet/GameplayStatics.h"

ESaveLoadError UN3DStatics::SaveNonogram(const UObject* WorldContextObject, const FString& NonogramName, const TMap<int32, FColor> Solution)
{
	UN3DGameInstance* Instance = GetN3DGameInstance(WorldContextObject);
	if (!Instance || Instance->GetMode() != EGameMode::Editor)
	{
		return ESaveLoadError::NotInEditMode;
	}

	if (NonogramName.IsEmpty())
	{
		return ESaveLoadError::InvalidName;
	}

	if (Solution.IsEmpty())
	{
		return ESaveLoadError::EmptySolution;
	}

	TArray<FString> StringSolution;
	StringSolution.Add("---,CubeIndex,FinalColor");
	int i = 0;
	for (const TPair<int32, FColor>& Cube : Solution)
	{
		FString Line = FString::FromInt(i) + ",\"" + FString::FromInt(Cube.Key) + "\",\"" + Cube.Value.ToString() + "\"";

		i++;
		StringSolution.Add(Line);
	}

	// TODO save also size

	FString Path;
	const FString NonogramFileName = NonogramName + FString(".csv");

#if PLATFORM_WINDOWS
	if (const UGeneralProjectSettings* ProjectSettings = GetDefault<UGeneralProjectSettings>())
	{
		Path = FPaths::Combine(FPlatformProcess::UserDir(), ProjectSettings->ProjectName, CREATED_NONOGRAMS, NonogramFileName);
	}
	else
	{
		Path = FPaths::Combine(FPlatformProcess::UserDir(), DEFAULT_PROJECT_NAME, CREATED_NONOGRAMS, NonogramFileName);
	}
#else
#error "Unsupported platform!"
	return ESaveLoadError::UnsupportedPlatform;
#endif

	if (Path.IsEmpty())
	{
		return ESaveLoadError::InvalidPath;
	}

	// TODO check if file doesn already exist

	if (FFileHelper::SaveStringArrayToFile(StringSolution, *Path))
	{
		return ESaveLoadError::Success;
	}

	return ESaveLoadError::FailedToSave;
}

void UN3DStatics::AddNonogramEditorColor(const UObject* WorldContextObject, const FLinearColor& Color)
{
	if (UN3DGameInstance* Instance = GetN3DGameInstance(WorldContextObject))
	{
		if (UN3DSaveSubsystem* SaveSubsystem = Instance->GetSubsystem<UN3DSaveSubsystem>())
		{
			SaveSubsystem->AddEditorColor(Color);
		}
	}
}

void UN3DStatics::GetNonogrameditorColors(const UObject* WorldContextObject, TArray<FLinearColor>& EditorColors)
{
	if (UN3DGameInstance* Instance = GetN3DGameInstance(WorldContextObject))
	{
		if (UN3DSaveSubsystem* SaveSubsystem = Instance->GetSubsystem<UN3DSaveSubsystem>())
		{
			EditorColors = SaveSubsystem->GetEditorColors();
			return;
		}
	}

	EditorColors = TArray<FLinearColor>();
}

bool UN3DStatics::GetNonogramName(const UObject* WorldContextObject, const int Index, FString& NonogramName)
{
	FNonogram Nonogram;
	if (GetNonogram(WorldContextObject, Index, Nonogram))
	{
		NonogramName = Nonogram.NonogramName;
		return true;
	}

	NonogramName = "";
	return false;
}

bool UN3DStatics::GetNonogram(const UObject* WorldContextObject, const int Index, FNonogram& Nonogram)
{
	if (UN3DGameInstance* Instance = GetN3DGameInstance(WorldContextObject))
	{
		if (UN3DSaveSubsystem* SaveSubsystem = Instance->GetSubsystem<UN3DSaveSubsystem>())
		{
			if (SaveSubsystem->GetNonograms()->Nonograms.IsValidIndex(Index))
			{
				Nonogram = SaveSubsystem->GetNonograms()->Nonograms[Index];
				return true;
			}
		}
	}

	Nonogram = FNonogram();
	return false;
}

UN3DGameInstance* UN3DStatics::GetN3DGameInstance(const UObject* WorldContextObject)
{
	return Cast<UN3DGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
}
