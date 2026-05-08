// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MapGeneratorWrapper.generated.h"

struct MapGenMapData;

USTRUCT(BlueprintType)
struct FMapGenTerrainTypeDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Type")
	FString Name = TEXT("Terrain");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Type", meta = (ClampMin = "-10.0", ClampMax = "1000.0"))
	float MaxHeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Type", meta = (ClampMin = "-5.0", ClampMax = "5.0"))
	float BaseHeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Type")
	bool bIsWater = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate Range", meta = (ClampMin = "0.0", ClampMax = "1.0", ToolTip = "0=arctic 1=tropical"))
	float MinTemperature = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate Range", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxTemperature = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate Range", meta = (ClampMin = "0.0", ClampMax = "1.0", ToolTip = "0=arid 1=wet"))
	float MinMoisture = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate Range", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxMoisture = 0.0f;
};

USTRUCT(BlueprintType)
struct FMapGenClimateSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate", meta = (ClampMin = "0.0", ClampMax = "1.0", ToolTip = "Normalized row of equator (0=top, 1=bottom, 0.5=center)"))
	float EquatorNormalizedRow = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate", meta = (ClampMin = "0.0", ClampMax = "2.0", ToolTip = "Temperature drop per unit of normalized elevation above sea level"))
	float ElevationTempPenalty = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate", meta = (ClampMin = "0.0", ClampMax = "0.5", ToolTip = "Perlin noise strength added to temperature"))
	float TemperatureNoiseStrength = 0.12f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate", meta = (ClampMin = "0.0", ClampMax = "0.5", ToolTip = "Perlin noise strength added to moisture"))
	float MoistureNoiseStrength = 0.15f;
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
	int32 Terrain = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Map Generation", meta = (ToolTip = "0=arctic 1=tropical"))
	float Temperature = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Map Generation", meta = (ToolTip = "0=arid 1=wet"))
	float Moisture = 0.0f;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Types")
	TArray<FMapGenTerrainTypeDefinition> TerrainTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise Settings")
	FMapGenTerrainNoiseSettings NoiseSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climate Settings")
	FMapGenClimateSettings ClimateSettings;

	UPROPERTY(BlueprintReadOnly, Category = "Map Generation")
	TArray<FMapGenTileData> Tiles;

	UPROPERTY(BlueprintAssignable, Category = "Map Generation")
	FOnMapGeneratedDelegate OnMapGenerated;

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	bool GenerateMap();

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	void RegenerateMap();

	UFUNCTION(BlueprintPure, Category = "Map Generation")
	FString GetTerrainName(int32 TerrainIndex) const;

	UFUNCTION(BlueprintPure, Category = "Map Generation")
	FVector GetTileLocalOffset(const FMapGenTileData& Tile) const;

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	void ResetTerrainTypesToDefaults();

	UFUNCTION(BlueprintCallable, Category = "Map Generation")
	void ResetClimateSettingsToDefaults();

private:
	MapGenMapData* CurrentMapData;

	void FreeCurrentMap() const;
};
