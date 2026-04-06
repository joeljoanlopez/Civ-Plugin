// Map Generator Wrapper for Unreal Engine

#include "MapGeneratorWrapper.h"
#include "DrawDebugHelpers.h"
#include "api/MapGenerationAPI.h"

AMapGeneratorWrapper::AMapGeneratorWrapper()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentMapData = new MapGenMapData();
	CurrentMapData->tiles = nullptr;
	CurrentMapData->width = 0;
	CurrentMapData->height = 0;
	CurrentMapData->tileCount = 0;
}

void AMapGeneratorWrapper::BeginPlay()
{
	Super::BeginPlay();
	GenerateMap();
}

void AMapGeneratorWrapper::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	FreeCurrentMap();
	delete CurrentMapData;
	CurrentMapData = nullptr;
}

#if WITH_EDITOR
void AMapGeneratorWrapper::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	// Auto-regenerate when properties change in editor
	if (PropertyChangedEvent.Property != nullptr)
	{
		FName PropertyName = PropertyChangedEvent.Property->GetFName();
		if (PropertyName == GET_MEMBER_NAME_CHECKED(AMapGeneratorWrapper, Width) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(AMapGeneratorWrapper, Height) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(AMapGeneratorWrapper, Seed) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(AMapGeneratorWrapper, PlateCount) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(AMapGeneratorWrapper, LandRatio) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(AMapGeneratorWrapper, NoiseOctaves))
		{
			RegenerateMap();
		}
	}
}
#endif

bool AMapGeneratorWrapper::GenerateMap()
{
	FreeCurrentMap();

	// Call the C API
	int32 Result = MapGenGenerateMap(Width, Height, Seed, PlateCount, LandRatio, NoiseOctaves, CurrentMapData);

	if (Result == 0)
	{
		Tiles.Empty();
		UE_LOG(LogTemp, Error, TEXT("Failed to generate map"));
		return false;
	}

	// Convert C structs to Unreal structs
	Tiles.SetNum(CurrentMapData->tileCount);
	for (int32 i = 0; i < CurrentMapData->tileCount; ++i)
	{
		const MapGenTileData& SrcTile = CurrentMapData->tiles[i];
		FMapGenTileData& DstTile = Tiles[i];

		DstTile.Q = SrcTile.q;
		DstTile.R = SrcTile.r;
		DstTile.TectonicPlateId = SrcTile.tectonicPlateId;
		DstTile.bIsLand = SrcTile.isLand != 0;
		DstTile.Height = SrcTile.height;
		DstTile.Terrain = static_cast<ETerrainType>(SrcTile.terrain);
	}

	DrawDebugHexGrid();

	UE_LOG(LogTemp, Log, TEXT("Generated map with %d tiles"), Tiles.Num());
	return true;
}

void AMapGeneratorWrapper::RegenerateMap()
{
	GenerateMap();
}

void AMapGeneratorWrapper::FreeCurrentMap()
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

FLinearColor AMapGeneratorWrapper::GetTerrainColor(ETerrainType Terrain) const
{
	switch (Terrain)
	{
	case ETerrainType::DeepOcean:
		return FLinearColor(0.1f, 0.2f, 0.5f, 1.0f);
	case ETerrainType::Water:
		return FLinearColor(0.2f, 0.4f, 0.8f, 1.0f);
	case ETerrainType::Coast:
		return FLinearColor(0.8f, 0.8f, 0.6f, 1.0f);
	case ETerrainType::Land:
		return FLinearColor(0.3f, 0.7f, 0.3f, 1.0f);
	case ETerrainType::Mountain:
		return FLinearColor(0.6f, 0.6f, 0.6f, 1.0f);
	default:
		return FLinearColor(1.0f, 0.0f, 1.0f, 1.0f); // Magenta for unknown
	}
}

void AMapGeneratorWrapper::DrawDebugHexGrid()
{
	if (!GetWorld() || Tiles.Num() == 0)
	{
		return;
	}

	const float HexSize = 100.0f; // 1 meter in Unreal units
	const float Duration = -1.0f; // Persistent debug lines

	for (const FMapGenTileData& Tile : Tiles)
	{
		// Convert hex coordinates to world position
		const float X = HexSize * FMath::Sqrt(3.0f) * (Tile.Q + Tile.R / 2.0f);
		const float Y = -HexSize * (3.0f / 2.0f) * Tile.R; // Negate Y so (0,0) is at top-left
		const FVector Center = GetActorLocation() + FVector(X, Y, 0.0f);

		// Draw hexagon
		FLinearColor Color = GetTerrainColor(Tile.Terrain);
		DrawHexagon(Center, HexSize, Color);

		// Draw labels if enabled
#if WITH_EDITOR
		if (bShowTerrain || bShowPlateId || bShowHeight || bShowCoordinates)
		{
			FString Label;
			if (bShowTerrain)
			{
				Label += FString::Printf(TEXT("%s\n"), *UEnum::GetDisplayValueAsText(Tile.Terrain).ToString());
			}
			if (bShowPlateId)
			{
				Label += FString::Printf(TEXT("Plate: %d\n"), Tile.TectonicPlateId);
			}
			if (bShowHeight)
			{
				Label += FString::Printf(TEXT("H: %.2f\n"), Tile.Height);
			}
			if (bShowCoordinates)
			{
				Label += FString::Printf(TEXT("(%d,%d)"), Tile.Q, Tile.R);
			}

			DrawDebugString(GetWorld(), Center + FVector(0, 0, 10), Label, nullptr, FColor::White, Duration, false);
		}
#endif
	}
}

void AMapGeneratorWrapper::DrawHexagon(const FVector& Center, float Size, const FLinearColor& Color) const
{
	if (!GetWorld())
	{
		return;
	}

	TArray<FVector> Vertices;
	Vertices.SetNum(6);

	// Calculate hexagon vertices
	for (int32 i = 0; i < 6; ++i)
	{
		const float AngleDeg = 60.0f * i + 30.0f;
		const float AngleRad = FMath::DegreesToRadians(AngleDeg);
		Vertices[i] = Center + FVector(
			Size * FMath::Cos(AngleRad),
			Size * FMath::Sin(AngleRad),
			0.0f
		);
	}

	// Draw hexagon edges
	const FColor EdgeColor = FColor::Black;
	const float Duration = -1.0f;
	const float Thickness = 2.0f;

	for (int32 i = 0; i < 6; ++i)
	{
		int32 Next = (i + 1) % 6;
		DrawDebugLine(GetWorld(), Vertices[i], Vertices[Next], EdgeColor, true, Duration, 0, Thickness);
	}

	// Draw filled hexagon (using triangles from center)
	const FColor FillColor = Color.ToFColor(true);
	for (int32 i = 0; i < 6; ++i)
	{
		int32 Next = (i + 1) % 6;
		DrawDebugLine(GetWorld(), Center, Vertices[i], FillColor, true, Duration, 0, 1.0f);
		DrawDebugLine(GetWorld(), Center, Vertices[Next], FillColor, true, Duration, 0, 1.0f);
	}
}
