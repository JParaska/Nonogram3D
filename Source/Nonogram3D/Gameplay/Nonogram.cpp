// Created by Jan Paraska. All rights reserved


#include "Nonogram.h"

#include "Components/InstancedStaticMeshComponent.h"

ANonogram::ANonogram()
{
	PrimaryActorTick.bCanEverTick = false;

	CubeInstances = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Cubes"));
	RootComponent = CubeInstances;
}

void ANonogram::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(CubeInstances))
	{
		return;
	}

	// TODO register

	// TODO get puzzle info

	/// Temp
	const int Size = 10;
	for (uint8 x = 0; x < Size; x++)
	{
		for (uint8 y = 0; y < Size; y++)
		{
			for (uint8 z = 0; z < Size; z++)
			{
				FTransform InstanceTransform(FRotator::ZeroRotator, FVector(x * 100.0f, y * 100.0f, z * 100.0f), CubeScale);
				int32 CubeIndex = CubeInstances->AddInstance(InstanceTransform, true);
				Cubes.Add(FIntVector(x, y, z), CubeIndex);
			}
		}
	}
}

