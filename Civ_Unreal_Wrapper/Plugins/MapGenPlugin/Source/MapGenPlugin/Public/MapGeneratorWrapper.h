// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MapGeneratorWrapper.generated.h"

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
	float LandBaseHeight = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Base Heights")
	float WaterBaseHeight = -0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Base Heights")
	float CoastLandHeight = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Base Heights")
	float CoastWaterHeight = -0.05f;
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
	Water     = 1 UMETA(DisplayName = "Water"),
	Coast     = 2 UMETA(DisplayName = "Coast"),
	Land      = 3 UMETA(DisplayName = "Land"),
	Mountain  = 4 UMETA(DisplayName = "Mountain")
};

USTRUCT(BlueprintType)
struct FMapGenTileData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Map Generation")
	int32 Q = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Map Generation")
	int32 R = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Map Generation")
	int32 TectonicPlateId = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Map Generation")
	bool bIsLand = false;

	UPROPERTY(BlueprintReadOnly, Category = "Map Generation")
	float Height = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Map Generation")
	ETerrainType Terrain = ETerrainType::DeepOcean;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMapGeneratedDelegate, const TArray<FMapGenTileData>&, Tiles);

UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class MAPGENPLUGIN_API UMapGeneratorWrapper : public UObject
{
	GENERATED_BODY()

public:
	UMapGeneratorWrapper();
	virtual void BeginDestroy() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings", meta = (ClampMin = "4", ClampMax = "50"))
	int32 Width = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings", meta = (ClampMin = "4", ClampMax = "50"))
	int32 Height = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings")
	int32 Seed = 1234;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid Settings", meta = (ClampMin = "1.0"))
	float TileSize = 100.0f;

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

	UPROPERTY(BlueprintReadOnly, Category = "Map Generation")
	TArray<FMapGenTileData> Tiles;

	UPROPERTY(BlueprintAssignable, Category = "Map Generation")
	FOnMapGeneratedDelegate OnMapGenerated;

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	bool GenerateMap();

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	void RegenerateMap();

	// Returns grid-local offset (no world position). Caller adds actor location.
	UFUNCTION(BlueprintPure, Category = "Map Generation")
	FVector GetTileLocalOffset(const FMapGenTileData& Tile) const;

private:
	MapGenMapData* CurrentMapData;

	void FreeCurrentMap() const;
};
