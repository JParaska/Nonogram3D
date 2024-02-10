// Created by Jan Paraska. All rights reserved


#include "N3DStatics.h"

#include "N3DGameInstance.h"
#include "N3DNonogramList.h"
#include "N3DSaveSubsystem.h"
#include "Nonogram3DTypes.h"

#include "GeneralProjectSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/LocalTimestampDirectoryVisitor.h"
#include "Serialization/BufferArchive.h"

ESaveLoadError UN3DStatics::SaveNonogram(const UObject* WorldContextObject, const FString& NonogramName, FIntVector Size, TMap<int32,FColor> Solution)
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

	if (Size == FIntVector::ZeroValue)
	{
		return ESaveLoadError::InvalidSize;
	}

	FString Path;
	const FString NonogramFileName = NonogramName + FString(".n3d");

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

	FBufferArchive Archive;
	Archive << Size;
	Archive << Solution;

	// TODO check if file doesn already exist

	if (FFileHelper::SaveArrayToFile(Archive, *Path))
	{
		Archive.FlushCache();
		Archive.Empty();

		// Synch save game
		if (UN3DSaveSubsystem* SaveSubsystem = Instance->GetSubsystem<UN3DSaveSubsystem>())
		{
			SaveSubsystem->ResolveCreatedNonograms();
		}

		return ESaveLoadError::Success;
	}

	Archive.FlushCache();
	Archive.Empty();

	return ESaveLoadError::FailedToSave;
}

void UN3DStatics::FindMyCreatedNonograms(const UObject* WorldContextObject, TArray<FNonogram>& MyCreatedNonograms)
{
	FString Path;

#if PLATFORM_WINDOWS
	if (const UGeneralProjectSettings* ProjectSettings = GetDefault<UGeneralProjectSettings>())
	{
		Path = FPaths::Combine(FPlatformProcess::UserDir(), ProjectSettings->ProjectName, CREATED_NONOGRAMS);
	}
	else
	{
		Path = FPaths::Combine(FPlatformProcess::UserDir(), DEFAULT_PROJECT_NAME, CREATED_NONOGRAMS);
	}
#else
#error "Unsupported platform!"
	//return ESaveLoadError::UnsupportedPlatform;
#endif

	IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FLocalTimestampDirectoryVisitor Visitor(PlatformFile, {}, {}, false);
	PlatformFile.IterateDirectory(*Path, Visitor);

	TArray<uint8> BinaryArray;
	for (TMap<FString, FDateTime>::TIterator TimestampIt(Visitor.FileTimes); TimestampIt; ++TimestampIt)
	{
		const FString FilePath = TimestampIt.Key();		
		const FString FileName = FPaths::GetCleanFilename(FilePath);

		// Load file
		if (!FFileHelper::LoadFileToArray(BinaryArray, *FilePath))
		{
			continue;
		}

		FMemoryReader FromBinary = FMemoryReader(BinaryArray, true); //true, free data after done
		FromBinary.Seek(0);

		// Extract size
		FIntVector Size;
		FromBinary << Size;

		// Extract solution
		TMap<int32, FColor> Solution;
		FromBinary << Solution;

		UE_LOG(LogTemp, Warning, TEXT("Loaded %s with size %s and %i cubes"), *FileName, *Size.ToString(), Solution.Num());

		// Empty & Close Buffer 
		BinaryArray.Reset();
		FromBinary.FlushCache();
		FromBinary.Close();

		if (Size != FIntVector::ZeroValue && !Solution.IsEmpty())
		{
			FNonogram MyCreatedNonogram;
			MyCreatedNonogram.NonogramName = FileName; // TODO remove suffix
			MyCreatedNonogram.Size = Size;
			MyCreatedNonogram.Solution = Solution;

			MyCreatedNonograms.Add(MyCreatedNonogram);
		}
	}

	if (!MyCreatedNonograms.IsEmpty())
	{
		MyCreatedNonograms.StableSort([](const FNonogram& Lhs, const FNonogram& Rhs) -> bool {
			return Lhs.NonogramName < Rhs.NonogramName;
		});
	}
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

bool UN3DStatics::GetCreatedNonogram(const UObject* WorldContextObject, const FString& NonogramName, FNonogram& Nonogram)
{
	if (UN3DGameInstance* Instance = GetN3DGameInstance(WorldContextObject))
	{
		if (UN3DSaveSubsystem* SaveSubsystem = Instance->GetSubsystem<UN3DSaveSubsystem>())
		{
			if (SaveSubsystem->GetMyCreatedNonograms().Contains(NonogramName))
			{
				Nonogram = SaveSubsystem->GetMyCreatedNonograms()[NonogramName];
				return true;
			}
		}
	}

	return false;
}

UN3DGameInstance* UN3DStatics::GetN3DGameInstance(const UObject* WorldContextObject)
{
	return Cast<UN3DGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
}
