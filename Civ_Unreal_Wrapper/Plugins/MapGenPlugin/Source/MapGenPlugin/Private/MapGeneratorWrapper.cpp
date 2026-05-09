// Map Generator Wrapper for Unreal Engine

#include "MapGeneratorWrapper.h"
#include "api/MapGenerationAPI.h"
#include <cstring>

UMapGeneratorWrapper::UMapGeneratorWrapper()
{
	CurrentMapData = new MapGenMapData();
	CurrentMapData->tiles = nullptr;
	CurrentMapData->width = 0;
	CurrentMapData->height = 0;
	CurrentMapData->tileCount = 0;

	ResetTerrainTypesToDefaults();
}

void UMapGeneratorWrapper::BeginDestroy()
{
	FreeCurrentMap();
	delete CurrentMapData;
	CurrentMapData = nullptr;
	Super::BeginDestroy();
}

bool UMapGeneratorWrapper::GenerateMap()
{
	FreeCurrentMap();

	TArray<MapGenTerrainTypeDefinition> CTypes;
	CTypes.SetNum(TerrainTypes.Num());
	for (int32 i = 0; i < TerrainTypes.Num(); ++i)
	{
		FMemory::Memzero(CTypes[i].name, 64);
		FTCHARToUTF8 Converter(*TerrainTypes[i].Name);
		FCStringAnsi::Strncpy(CTypes[i].name, Converter.Get(), 63);
		CTypes[i].maxHeight       = TerrainTypes[i].MaxHeight;
		CTypes[i].baseHeight      = TerrainTypes[i].BaseHeight;
		CTypes[i].isWater         = TerrainTypes[i].bIsWater ? 1 : 0;
		CTypes[i].minTemperature  = TerrainTypes[i].MinTemperature;
		CTypes[i].maxTemperature  = TerrainTypes[i].MaxTemperature;
		CTypes[i].minMoisture     = TerrainTypes[i].MinMoisture;
		CTypes[i].maxMoisture     = TerrainTypes[i].MaxMoisture;
	}

	TerrainNoiseSettings CNoiseSettings;
	CNoiseSettings.noiseScale          = NoiseSettings.NoiseScale;
	CNoiseSettings.initialAmplitude    = NoiseSettings.InitialAmplitude;
	CNoiseSettings.initialFrequency    = NoiseSettings.InitialFrequency;
	CNoiseSettings.amplitudeDecay      = NoiseSettings.AmplitudeDecay;
	CNoiseSettings.frequencyMultiplier = NoiseSettings.FrequencyMultiplier;
	CNoiseSettings.noiseStrength       = NoiseSettings.NoiseStrength;

	MapGenClimateSettings CClimateSettings;
	CClimateSettings.equatorNormalizedRow     = ClimateSettings.EquatorNormalizedRow;
	CClimateSettings.elevationTempPenalty     = ClimateSettings.ElevationTempPenalty;
	CClimateSettings.temperatureNoiseStrength = ClimateSettings.TemperatureNoiseStrength;
	CClimateSettings.moistureNoiseStrength    = ClimateSettings.MoistureNoiseStrength;

	MapGenTerrainTypeDefinition* TypesPtr = CTypes.Num() > 0 ? CTypes.GetData() : nullptr;
	const int32 TypesCount = CTypes.Num();

	int32 Result = MapGenGenerateMap(Width, Height, Seed, PlateCount, LandRatio, NoiseOctaves,
		TypesPtr, TypesCount, &CNoiseSettings, &CClimateSettings, CurrentMapData);

	if (Result == 0)
	{
		Tiles.Empty();
		return false;
	}

	Tiles.SetNum(CurrentMapData->tileCount);
	for (int32 i = 0; i < CurrentMapData->tileCount; ++i)
	{
		const MapGenTileData& Src = CurrentMapData->tiles[i];
		Tiles[i].Q               = Src.q;
		Tiles[i].R               = Src.r;
		Tiles[i].TectonicPlateId = Src.tectonicPlateId;
		Tiles[i].bIsLand         = Src.isLand != 0;
		Tiles[i].Height          = Src.height;
		Tiles[i].Terrain         = Src.terrain;
		Tiles[i].Temperature     = Src.temperature;
		Tiles[i].Moisture        = Src.moisture;
	}

	OnMapGenerated.Broadcast(Tiles);
	return true;
}

void UMapGeneratorWrapper::RegenerateMap()
{
	GenerateMap();
}

FString UMapGeneratorWrapper::GetTerrainName(int32 TerrainIndex) const
{
	if (TerrainIndex >= 0 && TerrainIndex < TerrainTypes.Num())
		return TerrainTypes[TerrainIndex].Name;
	return FString::FromInt(TerrainIndex);
}

void UMapGeneratorWrapper::ResetTerrainTypesToDefaults()
{
	const int32 Count = MapGenGetDefaultTerrainTypeCount();
	TArray<MapGenTerrainTypeDefinition> Defaults;
	Defaults.SetNum(Count);
	MapGenGetDefaultTerrainTypes(Defaults.GetData(), Count);

	TerrainTypes.SetNum(Count);
	for (int32 i = 0; i < Count; ++i)
	{
		TerrainTypes[i].Name            = UTF8_TO_TCHAR(Defaults[i].name);
		TerrainTypes[i].MaxHeight       = Defaults[i].maxHeight;
		TerrainTypes[i].BaseHeight      = Defaults[i].baseHeight;
		TerrainTypes[i].bIsWater        = Defaults[i].isWater != 0;
		TerrainTypes[i].MinTemperature  = Defaults[i].minTemperature;
		TerrainTypes[i].MaxTemperature  = Defaults[i].maxTemperature;
		TerrainTypes[i].MinMoisture     = Defaults[i].minMoisture;
		TerrainTypes[i].MaxMoisture     = Defaults[i].maxMoisture;
	}
}

void UMapGeneratorWrapper::ResetClimateSettingsToDefaults()
{
	MapGenClimateSettings Defaults = MapGenGetDefaultClimateSettings();
	ClimateSettings.EquatorNormalizedRow     = Defaults.equatorNormalizedRow;
	ClimateSettings.ElevationTempPenalty     = Defaults.elevationTempPenalty;
	ClimateSettings.TemperatureNoiseStrength = Defaults.temperatureNoiseStrength;
	ClimateSettings.MoistureNoiseStrength    = Defaults.moistureNoiseStrength;
}

void UMapGeneratorWrapper::FreeCurrentMap() const
{
	if (CurrentMapData && CurrentMapData->tiles != nullptr)
	{
		MapGenFreeMap(CurrentMapData);
		CurrentMapData->tiles = nullptr;
		CurrentMapData->width = 0;
		CurrentMapData->height = 0;
		CurrentMapData->tileCount = 0;
	}
}

FVector UMapGeneratorWrapper::GetTileLocalOffset(const FMapGenTileData& Tile) const
{
	const float X = -TileSize * (3.0f / 2.0f) * Tile.R;
	const float Y = TileSize * FMath::Sqrt(3.0f) * (Tile.Q + Tile.R / 2.0f);
	return FVector(X, Y, 0.0f);
}
