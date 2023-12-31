// Created by Jan Paraska. All rights reserved


#include "N3DStatics.h"

#include "N3DGameInstance.h"

#include "GeneralProjectSettings.h"
#include "Kismet/GameplayStatics.h"

ESaveLoadError UN3DStatics::SaveNonogram(const UObject* WorldContextObject, const FString& NonogramName, const TMap<int32, FColor> Solution)
{
	UN3DGameInstance* Instance = Cast<UN3DGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
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
