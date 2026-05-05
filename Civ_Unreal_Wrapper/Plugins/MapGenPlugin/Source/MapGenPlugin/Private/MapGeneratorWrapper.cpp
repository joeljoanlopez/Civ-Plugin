// Map Generator Wrapper for Unreal Engine

#include "MapGeneratorWrapper.h"
#include "api/MapGenerationAPI.h"

UMapGeneratorWrapper::UMapGeneratorWrapper()
{
	CurrentMapData = new MapGenMapData();
	CurrentMapData->tiles = nullptr;
	CurrentMapData->width = 0;
	CurrentMapData->height = 0;
	CurrentMapData->tileCount = 0;
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

	TerrainThresholds CThresholds;
	CThresholds.deepOceanMax = Thresholds.DeepOceanMax;
	CThresholds.waterMax     = Thresholds.WaterMax;
	CThresholds.coastMax     = Thresholds.CoastMax;
	CThresholds.landMax      = Thresholds.LandMax;

	TerrainBaseHeights CBaseHeights;
	CBaseHeights.landBaseHeight  = BaseHeights.LandBaseHeight;
	CBaseHeights.waterBaseHeight = BaseHeights.WaterBaseHeight;

	TerrainNoiseSettings CNoiseSettings;
	CNoiseSettings.noiseScale          = NoiseSettings.NoiseScale;
	CNoiseSettings.initialAmplitude    = NoiseSettings.InitialAmplitude;
	CNoiseSettings.initialFrequency    = NoiseSettings.InitialFrequency;
	CNoiseSettings.amplitudeDecay      = NoiseSettings.AmplitudeDecay;
	CNoiseSettings.frequencyMultiplier = NoiseSettings.FrequencyMultiplier;
	CNoiseSettings.noiseStrength       = NoiseSettings.NoiseStrength;

	int32 Result = MapGenGenerateMap(Width, Height, Seed, PlateCount, LandRatio, NoiseOctaves,
		&CThresholds, &CBaseHeights, &CNoiseSettings, CurrentMapData);

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
		Tiles[i].Terrain         = static_cast<ETerrainType>(Src.terrain);
	}

	OnMapGenerated.Broadcast(Tiles);
	return true;
}

void UMapGeneratorWrapper::RegenerateMap()
{
	GenerateMap();
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
