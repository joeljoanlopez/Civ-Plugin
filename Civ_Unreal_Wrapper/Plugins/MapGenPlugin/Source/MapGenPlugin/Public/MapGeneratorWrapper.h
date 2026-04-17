// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Modules/ModuleManager.h"
#include "MapGeneratorWrapper.generated.h"

class FMapGenPluginModule : public IModuleInterface
{
};

// Forward declare C struct to avoid including C header in .h file
struct MapGenMapData;

USTRUCT(BlueprintType)
struct FMapGenTerrainThresholds
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Thresholds")
	float DeepOceanMax = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Thresholds")
	float WaterMax = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Thresholds")
	float CoastMax = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Thresholds")
	float LandMax = 0.6f;
};

USTRUCT(BlueprintType)
struct FMapGenTerrainBaseHeights
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Base Heights")
	float LandBaseHeight = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Base Heights")
	float WaterBaseHeight = -0.2f;
};

USTRUCT(BlueprintType)
struct FMapGenTerrainNoiseSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	float NoiseScale = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	float InitialAmplitude = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	float InitialFrequency = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	float AmplitudeDecay = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	float FrequencyMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	float NoiseStrength = 0.5f;
};

UENUM(BlueprintType)
enum class ETerrainType : uint8
{
	DeepOcean = 0 UMETA(DisplayName = "Deep Ocean"),
	Water = 1 UMETA(DisplayName = "Water"),
	Coast = 2 UMETA(DisplayName = "Coast"),
	Land = 3 UMETA(DisplayName = "Land"),
	Mountain = 4 UMETA(DisplayName = "Mountain")
};

USTRUCT(BlueprintType)
struct FMapGenTileData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Map Generation")
	int32 Q;

	UPROPERTY(BlueprintReadOnly, Category = "Map Generation")
	int32 R;

	UPROPERTY(BlueprintReadOnly, Category = "Map Generation")
	int32 TectonicPlateId;

	UPROPERTY(BlueprintReadOnly, Category = "Map Generation")
	bool bIsLand;

	UPROPERTY(BlueprintReadOnly, Category = "Map Generation")
	float Height;

	UPROPERTY(BlueprintReadOnly, Category = "Map Generation")
	ETerrainType Terrain;

	FMapGenTileData()
		: Q(0), R(0), TectonicPlateId(0), bIsLand(false), Height(0.0f), Terrain(ETerrainType::DeepOcean)
	{
	}
};

UCLASS(Blueprintable, BlueprintType)
class MAPGENPLUGIN_API AMapGeneratorWrapper : public AActor
{
	GENERATED_BODY()
	
public:	
	AMapGeneratorWrapper();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings", meta = (ClampMin = "4", ClampMax = "50"))
	int32 Width = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings", meta = (ClampMin = "4", ClampMax = "50"))
	int32 Height = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	int32 Seed = 1234;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Config", meta = (ClampMin = "2", ClampMax = "15"))
	int32 PlateCount = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Config", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LandRatio = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Random Config", meta = (ClampMin = "1", ClampMax = "5"))
	int32 NoiseOctaves = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Thresholds")
	FMapGenTerrainThresholds Thresholds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Base Heights")
	FMapGenTerrainBaseHeights BaseHeights;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	FMapGenTerrainNoiseSettings NoiseSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
	bool bShowTerrain = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
	bool bShowPlateId = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
	bool bShowHeight = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
	bool bShowCoordinates = false;

	UPROPERTY(BlueprintReadOnly, Category = "Map Generation")
	TArray<FMapGenTileData> Tiles;

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	bool GenerateMap();

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	void RegenerateMap();

	UFUNCTION(BlueprintPure, Category = "Map Generation")
	static FLinearColor GetTerrainColor(ETerrainType Terrain);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	MapGenMapData* CurrentMapData;
	void FreeCurrentMap() const;
	void DrawDebugHexGrid();
	void DrawHexagon(const FVector& Center, float Size, const FLinearColor& Color) const;
};
