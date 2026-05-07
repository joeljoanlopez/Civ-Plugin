// Map Tile Instancer Component for Unreal Engine

#include "MapTileInstancerComponent.h"
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

	if (PropertyChangedEvent.Property == nullptr)
		return;

	FName PropertyName = PropertyChangedEvent.Property->GetFName();

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UMapTileInstancerComponent, bSpawn3DObjects) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UMapTileInstancerComponent, TerrainMeshes) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UMapTileInstancerComponent, HeightScale) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(UMapTileInstancerComponent, TileScale))
	{
		if (bSpawn3DObjects && Wrapper)
			Wrapper->GenerateMap();
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
	if (bSpawn3DObjects)
		SpawnTiles(Tiles);
	else
		DestroySpawnedTiles();
}

void UMapTileInstancerComponent::SpawnTiles(const TArray<FMapGenTileData>& Tiles)
{
	DestroySpawnedTiles();
	if (!Wrapper || TerrainMeshes.Num() == 0)
		return;

	AActor* Owner = GetOwner();
	USceneComponent* Root = Owner->GetRootComponent();

	for (auto& [TerrainIndex, MeshList] : TerrainMeshes)
	{
		for (UStaticMesh* Mesh : MeshList.Meshes)
		{
			if (!Mesh || MeshHISMMap.Contains(Mesh))
				continue;

			UHierarchicalInstancedStaticMeshComponent* HISM =
				NewObject<UHierarchicalInstancedStaticMeshComponent>(Owner);
			HISM->SetupAttachment(Root);
			HISM->SetStaticMesh(Mesh);
			HISM->RegisterComponent();
			MeshHISMMap.Add(Mesh, HISM);
		}
	}

	for (const FMapGenTileData& Tile : Tiles)
	{
		const FTerrainMeshList* MeshList = TerrainMeshes.Find(Tile.Terrain);
		if (!MeshList || MeshList->Meshes.Num() == 0)
			continue;

		const int32 MeshIdx = FMath::RandRange(0, MeshList->Meshes.Num() - 1);
		UStaticMesh* ChosenMesh = MeshList->Meshes[MeshIdx];
		if (!ChosenMesh)
			continue;

		UHierarchicalInstancedStaticMeshComponent** HISM = MeshHISMMap.Find(ChosenMesh);
		if (!HISM)
			continue;

		FVector Pos = GetTileWorldPosition(Tile);
		Pos.Z = Tile.Height * HeightScale;
		FTransform InstanceTransform(FRotator(0.0f, 90.0f, 0.0f), Pos, FVector(TileScale));
		(*HISM)->AddInstance(InstanceTransform);
	}
}

void UMapTileInstancerComponent::DestroySpawnedTiles()
{
	for (auto& [Mesh, HISM] : MeshHISMMap)
	{
		if (HISM)
			HISM->DestroyComponent();
	}
	MeshHISMMap.Empty();
}

FVector UMapTileInstancerComponent::GetTileWorldPosition(const FMapGenTileData& Tile) const
{
	return GetOwner()->GetActorLocation() + Wrapper->GetTileLocalOffset(Tile);
}
