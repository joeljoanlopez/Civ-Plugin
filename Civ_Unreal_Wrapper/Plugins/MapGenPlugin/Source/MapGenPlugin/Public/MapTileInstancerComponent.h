// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MapGeneratorWrapper.h"
#include "MapTileInstancerComponent.generated.h"

class UStaticMesh;
class UHierarchicalInstancedStaticMeshComponent;

USTRUCT(BlueprintType)
struct FTerrainMeshList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UStaticMesh*> Meshes;
};

UCLASS(ClassGroup=(MapGen), meta=(BlueprintSpawnableComponent))
class MAPGENPLUGIN_API UMapTileInstancerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMapTileInstancerComponent();

	// Set this to a UMapGeneratorWrapper Blueprint or leave null to use default settings.
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "Map Generation")
	UMapGeneratorWrapper* Wrapper;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
	bool bShowTerrain = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
	bool bShowPlateId = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
	bool bShowHeight = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
	bool bShowCoordinates = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Spawning")
	bool bSpawn3DObjects = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Spawning")
	TMap<ETerrainType, FTerrainMeshList> TerrainMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Spawning", meta = (ClampMin = "0.0", ClampMax = "200.0"))
	float HeightScale = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Spawning", meta = (ClampMin = "0.5", ClampMax = "1.0"))
	float TileScale = 0.9f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	UPROPERTY()
	TMap<UStaticMesh*, UHierarchicalInstancedStaticMeshComponent*> MeshHISMMap;

	TArray<FMapGenTileData> CachedTiles;

	UFUNCTION()
	void OnMapGenerated(const TArray<FMapGenTileData>& Tiles);

	void EnsureWrapper();
	void SpawnTiles();
	void DestroySpawnedTiles();
	void DrawDebugHexGrid();
	void DrawHexagon(const FVector& Center, float Size, const FLinearColor& Color) const;
	static FLinearColor GetTerrainColor(ETerrainType Terrain);

	FVector GetTileWorldPosition(const FMapGenTileData& Tile) const;
};
