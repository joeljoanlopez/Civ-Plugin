// Copyright Epic Games, Inc. All Rights Reserved.

#include "CorePluginBPLibrary.h"
#include "CorePlugin.h"

UCorePluginBPLibrary::UCorePluginBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

float UCorePluginBPLibrary::CorePluginSampleFunction(float Param)
{
	return -1;
}

TArray<FVector> UCorePluginBPLibrary::GenerateLineOfPoints(float startX, float startY, float startZ, int32 count, float spacing, FString direction)
{
	CoreVector3 startPosition = CoreVector3(startX, startY, startZ);

	char dirChar = 'x';
	if (!direction.IsEmpty()) {
		dirChar = (char) direction[0];
	}

	std::vector<CoreVector3> linePositions = GenerateLine(startPosition, count, spacing, dirChar);

	TArray<FVector> result;

	for (const CoreVector3& pos : linePositions) {
		result.Add(FVector(pos.x, pos.y, pos.z));
	}

	return result;
}

void UCorePluginBPLibrary::SpawnActorsAtLocations(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, const TArray<FVector>& Locations, FRotator Rotation)
{
	if (!ActorClass) return;

	if (!WorldContextObject) return;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (const FVector& Location: Locations)
	{
		World->SpawnActor<AActor>(ActorClass, Location, Rotation, SpawnParams);
	}
}

