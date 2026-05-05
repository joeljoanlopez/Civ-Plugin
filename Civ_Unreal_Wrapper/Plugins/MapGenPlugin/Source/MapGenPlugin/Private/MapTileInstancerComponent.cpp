// Map Tile Instancer Component for Unreal Engine

#include "MapTileInstancerComponent.h"
#include "DrawDebugHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

UMapTileInstancerComponent::UMapTileInstancerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Wrapper = CreateDefaultSubobject<UMapGeneratorWrapper>(TEXT("MapGeneratorWrapper"));
}

void UMapTileInstancerComponent::BeginPlay()
{
	Super::BeginPlay();

	EnsureWrapper();
	InitHISMComponents();
	Wrapper->OnMapGenerated.AddDynamic(this, &UMapTileInstancerComponent::OnMapGenerated);
	Wrapper->GenerateMap();
}

void UMapTileInstancerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Wrapper)
		Wrapper->OnMapGenerated.RemoveDynamic(this, &UMapTileInstancerComponent::OnMapGenerated);

	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void UMapTileInstancerComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property == nullptr || CachedTiles.Num() == 0)
		return;

	FName PropertyName = PropertyChangedEvent.Property->GetFName();

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMapTileInstancerComponent, bShowTerrain) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UMapTileInstancerComponent, bShowPlateId) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UMapTileInstancerComponent, bShowHeight) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UMapTileInstancerComponent, bShowCoordinates))
	{
		DrawDebugHexGrid();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UMapTileInstancerComponent, bSpawn3DObjects) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UMapTileInstancerComponent, TileMesh) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UMapTileInstancerComponent, TileMaterial) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UMapTileInstancerComponent, HeightScale) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UMapTileInstancerComponent, TileScale))
	{
		if (bSpawn3DObjects)
			SpawnTiles();
		else
			DestroySpawnedTiles();
	}
}
#endif

void UMapTileInstancerComponent::EnsureWrapper()
{
	if (!Wrapper)
		Wrapper = NewObject<UMapGeneratorWrapper>(GetOwner());
}

void UMapTileInstancerComponent::OnMapGenerated(const TArray<FMapGenTileData>& Tiles)
{
	CachedTiles = Tiles;

	if (bSpawn3DObjects)
		SpawnTiles();
	else
	{
		DestroySpawnedTiles();
		DrawDebugHexGrid();
	}
}

void UMapTileInstancerComponent::InitHISMComponents()
{
	static const TCHAR* HISMNames[] = {
		TEXT("TileHISM_DeepOcean"), TEXT("TileHISM_Water"), TEXT("TileHISM_Coast"),
		TEXT("TileHISM_Land"), TEXT("TileHISM_Mountain")
	};

	AActor* Owner = GetOwner();
	USceneComponent* Root = Owner->GetRootComponent();

	for (int32 i = 0; i < 5; ++i)
	{
		UHierarchicalInstancedStaticMeshComponent* HISM =
			NewObject<UHierarchicalInstancedStaticMeshComponent>(Owner, HISMNames[i]);
		HISM->SetupAttachment(Root);
		HISM->RegisterComponent();
		TileInstanceComponents.Add(HISM);
	}
}

void UMapTileInstancerComponent::SpawnTiles()
{
	DestroySpawnedTiles();
	if (!TileMesh || !TileMaterial || !Wrapper)
		return;

	for (int32 i = 0; i < TileInstanceComponents.Num(); ++i)
	{
		UHierarchicalInstancedStaticMeshComponent* HISM = TileInstanceComponents[i];
		HISM->SetStaticMesh(TileMesh);
		UMaterialInstanceDynamic* Mat = UMaterialInstanceDynamic::Create(TileMaterial, GetOwner());
		Mat->SetVectorParameterValue(TEXT("TileColor"), GetTerrainColor(static_cast<ETerrainType>(i)));
		HISM->SetMaterial(0, Mat);
	}

	for (const FMapGenTileData& Tile : CachedTiles)
	{
		FVector Pos = GetTileWorldPosition(Tile);
		Pos.Z = Tile.Height * HeightScale;
		FTransform InstanceTransform(FRotator(0.0f, 90.0f, 0.0f), Pos, FVector(TileScale));

		const int32 TerrainIndex = static_cast<int32>(Tile.Terrain);
		if (TileInstanceComponents.IsValidIndex(TerrainIndex))
			TileInstanceComponents[TerrainIndex]->AddInstance(InstanceTransform);
	}
}

void UMapTileInstancerComponent::DestroySpawnedTiles()
{
	for (UHierarchicalInstancedStaticMeshComponent* HISM : TileInstanceComponents)
	{
		if (HISM)
			HISM->ClearInstances();
	}
}

void UMapTileInstancerComponent::DrawDebugHexGrid()
{
	UWorld* World = GetWorld();
	if (!World || CachedTiles.Num() == 0 || !Wrapper)
		return;

	FlushPersistentDebugLines(World);
	FlushDebugStrings(World);

	for (const FMapGenTileData& Tile : CachedTiles)
	{
		const FVector Center = GetTileWorldPosition(Tile);
		DrawHexagon(Center, Wrapper->TileSize, GetTerrainColor(Tile.Terrain));

#if WITH_EDITOR
		if (bShowTerrain || bShowPlateId || bShowHeight || bShowCoordinates)
		{
			TArray<FString> LabelParts;
			if (bShowTerrain)
				LabelParts.Add(UEnum::GetDisplayValueAsText(Tile.Terrain).ToString());
			if (bShowPlateId)
				LabelParts.Add(FString::Printf(TEXT("Plate: %d"), Tile.TectonicPlateId));
			if (bShowHeight)
				LabelParts.Add(FString::Printf(TEXT("H: %.2f"), Tile.Height));
			if (bShowCoordinates)
				LabelParts.Add(FString::Printf(TEXT("(%d,%d)"), Tile.Q, Tile.R));

			FString Label = FString::Join(LabelParts, TEXT("\n"));
			DrawDebugString(World, Center + FVector(0, 0, 10), Label, nullptr, FColor::White, -1.0f, false);
		}
#endif
	}
}

void UMapTileInstancerComponent::DrawHexagon(const FVector& Center, float Size, const FLinearColor& Color) const
{
	UWorld* World = GetWorld();
	if (!World)
		return;

	TArray<FVector> Vertices;
	Vertices.SetNum(6);
	for (int32 i = 0; i < 6; ++i)
	{
		const float AngleRad = FMath::DegreesToRadians(60.0f * i + 30.0f);
		Vertices[i] = Center + FVector(Size * FMath::Cos(AngleRad), Size * FMath::Sin(AngleRad), 0.0f);
	}

	const FColor EdgeColor = FColor::Black;
	const FColor FillColor = Color.ToFColor(true);

	for (int32 i = 0; i < 6; ++i)
	{
		const int32 Next = (i + 1) % 6;
		DrawDebugLine(World, Vertices[i], Vertices[Next], EdgeColor, true, -1.0f, 0, 2.0f);

		const FVector& V1 = Vertices[i];
		const FVector& V2 = Vertices[Next];
		for (float t = 0.1f; t < 1.0f; t += 0.1f)
			DrawDebugLine(World, Center, FMath::Lerp(V1, V2, t), FillColor, true, -1.0f, 0, 2.0f);
	}
}

FVector UMapTileInstancerComponent::GetTileWorldPosition(const FMapGenTileData& Tile) const
{
	return GetOwner()->GetActorLocation() + Wrapper->GetTileLocalOffset(Tile);
}

FLinearColor UMapTileInstancerComponent::GetTerrainColor(ETerrainType Terrain)
{
	switch (Terrain)
	{
	case ETerrainType::DeepOcean: return FLinearColor(0.1f, 0.2f, 0.5f, 1.0f);
	case ETerrainType::Water:     return FLinearColor(0.2f, 0.4f, 0.8f, 1.0f);
	case ETerrainType::Coast:     return FLinearColor(0.8f, 0.8f, 0.6f, 1.0f);
	case ETerrainType::Land:      return FLinearColor(0.3f, 0.7f, 0.3f, 1.0f);
	case ETerrainType::Mountain:  return FLinearColor(0.6f, 0.6f, 0.6f, 1.0f);
	default:                      return FLinearColor(1.0f, 0.0f, 1.0f, 1.0f);
	}
}
