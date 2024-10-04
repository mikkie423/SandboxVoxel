#include "TerrainTool.h"
#include "SandboxTerrainController.h"
#include "DrawDebugHelpers.h"

ATerrainTool::ATerrainTool()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ATerrainTool::BeginPlay()
{
    Super::BeginPlay();

    // Assuming there's only one TerrainController in the level
    for (TActorIterator<ASandboxTerrainController> It(GetWorld()); It; ++It)
    {
        TerrainController = *It;
        break;
    }

    if (!TerrainController)
    {
        UE_LOG(LogTemp, Error, TEXT("No TerrainController found in the level!"));
    }
}


void ATerrainTool::DigTerrain(const FHitResult& HitResult)
{
	if (!TerrainController) return;

	UE_LOG(LogTemp, Warning, TEXT("DigTerrain called"));

	PerformDigAction(HitResult.Location);
}

void ATerrainTool::FillTerrain(const FHitResult& HitResult)
{
    if (!TerrainController) return;

    UE_LOG(LogTemp, Warning, TEXT("FillTerrain called"));

    PerformFillAction(HitResult.Location);
}


void ATerrainTool::PerformDigAction(const FVector& Location)
{
    switch (CurrentShape)
    {
    case ShapeType::Circle:
        TerrainController->DigTerrainRoundHole(Location, DigFillRadius);
        break;
    case ShapeType::Cylinder:
        TerrainController->DigCylinder(Location, DigFillRadius, 100.0f); // Example length
        break;
    case ShapeType::Cube:
        TerrainController->DigTerrainCubeHole(Location, DigFillRadius);
        break;
    default:
        break;
    }

    // Optionally visualize the dig location
    DrawDebugSphere(GetWorld(), Location, 20.0f, 12, FColor::Green, false, 5.0f, 0, 1.0f);
}

void ATerrainTool::PerformFillAction(const FVector& Location)
{
    switch (CurrentShape)
    {
    case ShapeType::Circle:
        TerrainController->FillTerrainRound(Location, DigFillRadius, MaterialId);
        break;
    case ShapeType::Cylinder:
        // You may need a specific method for filling a cylinder
        break;
    case ShapeType::Cube:
        TerrainController->FillTerrainCube(Location, DigFillRadius, MaterialId);
        break;
    default:
        break;
    }

    DrawDebugSphere(GetWorld(), Location, 20.0f, 12, FColor::Red, false, 5.0f, 0, 1.0f);
}

void ATerrainTool::SetShape(ShapeType NewShape)
{
    CurrentShape = NewShape;
}

void ATerrainTool::SetSize(float NewSize)
{
    DigFillRadius = FMath::Clamp(NewSize, 10.0f, 500.0f);
}

