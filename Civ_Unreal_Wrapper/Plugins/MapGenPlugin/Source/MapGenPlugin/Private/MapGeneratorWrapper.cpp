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

	int32 Result = MapGenGenerateMap(Width, Height, Seed, PlateCount, LandRatio, NoiseOctaves, CurrentMapData);

	if (Result == 0)
	{
		Tiles.Empty();
		return false;
	}

	Tiles.SetNum(CurrentMapData->tileCount);
	for (int32 i = 0; i < CurrentMapData->tileCount; ++i)
	{
		const MapGenTileData& SourceTile = CurrentMapData->tiles[i];

		Tiles[i].Q = SourceTile.q;
		Tiles[i].R = SourceTile.r;
		Tiles[i].TectonicPlateId = SourceTile.tectonicPlateId;
		Tiles[i].bIsLand = SourceTile.isLand != 0;
		Tiles[i].Height = SourceTile.height;
		Tiles[i].Terrain = static_cast<ETerrainType>(SourceTile.terrain);
	}

	DrawDebugHexGrid();

	return true;
}

void AMapGeneratorWrapper::RegenerateMap()
{
	GenerateMap();
}

void AMapGeneratorWrapper::FreeCurrentMap() const
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

FLinearColor AMapGeneratorWrapper::GetTerrainColor(ETerrainType Terrain)
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
		return FLinearColor(1.0f, 0.0f, 1.0f, 1.0f);
	}
}

void AMapGeneratorWrapper::DrawDebugHexGrid()
{
	if (!GetWorld() || Tiles.Num() == 0)
	{
		return;
	}

	FlushPersistentDebugLines(GetWorld());

	for (const FMapGenTileData& Tile : Tiles)
	{
		constexpr float HexSize = 100.0f;
		const float X = HexSize * FMath::Sqrt(3.0f) * (Tile.Q + Tile.R / 2.0f);
		const float Y = -HexSize * (3.0f / 2.0f) * Tile.R;
		const FVector Center = GetActorLocation() + FVector(X, Y, 0.0f);

		FLinearColor Color = GetTerrainColor(Tile.Terrain);
		DrawHexagon(Center, HexSize, Color);

#if WITH_EDITOR
		if (bShowTerrain || bShowPlateId || bShowHeight || bShowCoordinates)
		{
			constexpr float Duration = -1.0f;
			TArray<FString> LabelParts;
			if (bShowTerrain)
			{
				LabelParts.Add(UEnum::GetDisplayValueAsText(Tile.Terrain).ToString());
			}
			if (bShowPlateId)
			{
				LabelParts.Add(FString::Printf(TEXT("Plate: %d"), Tile.TectonicPlateId));
			}
			if (bShowHeight)
			{
				LabelParts.Add(FString::Printf(TEXT("H: %.2f"), Tile.Height));
			}
			if (bShowCoordinates)
			{
				LabelParts.Add(FString::Printf(TEXT("(%d,%d)"), Tile.Q, Tile.R));
			}

			FString Label = FString::Join(LabelParts, TEXT("\n"));
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

	const FColor EdgeColor = FColor::Black;
	const FColor FillColor = Color.ToFColor(true);

	for (int32 i = 0; i < 6; ++i)
	{
		constexpr float EdgeThickness = 2.0f;
		constexpr float Duration = -1.0f;
		int32 Next = (i + 1) % 6;
		DrawDebugLine(GetWorld(), Vertices[i], Vertices[Next], EdgeColor, true, Duration, 0, EdgeThickness);

		const FVector& V1 = Vertices[i];
		const FVector& V2 = Vertices[Next];

		for (float t = 0.1f; t < 1.0f; t += 0.1f)
		{
			constexpr float FillThickness = 2.0f;
			FVector PointOnEdge = FMath::Lerp(V1, V2, t);
			DrawDebugLine(GetWorld(), Center, PointOnEdge, FillColor, true, Duration, 0, FillThickness);
		}
	}
}
