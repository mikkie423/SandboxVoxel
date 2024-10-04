#include "ShovelTool.h"
#include "SandboxTerrainController.h"

AShovelTool::AShovelTool()
{
    // Create the Static Mesh Component
    ShovelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShovelMesh"));

    // Set the Static Mesh (you can set this in the Blueprint, but you can also set a default here)
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/wheelbarrow-pushcart/source/Pushcart_shovel.Pushcart_shovel'"));
    if (MeshAsset.Succeeded())
    {
        ShovelMesh->SetStaticMesh(MeshAsset.Object);
    }

    // Attach the mesh to the Root Component (or another component, if needed)
    RootComponent = ShovelMesh;


    // You can customize shovel-specific settings here (e.g., DigFillRadius)
    DigFillRadius = 100.0f; // Example size for a shovel
}

void AShovelTool::PerformDigAction(const FVector& Location)
{
    if (TerrainController)
            {
                // Only dig dirt with the shovel
                // Example: Implement material filtering logic here based on HitResult
                // In this case, we assume that terrain can be dirt if it matches a specific condition.
        
                // Dig using a round hole, as typical for a shovel
                TerrainController->DigTerrainRoundHole(Location, DigFillRadius);
        
                // Debug draw
                DrawDebugSphere(GetWorld(), Location, 20.0f, 12, FColor::Green, false, 5.0f, 0, 1.0f);
            }
}

void AShovelTool::PerformFillAction(const FVector& Location)
{
    if (TerrainController)
            {
                // Filling behavior for the shovel
                TerrainController->FillTerrainRound(Location, DigFillRadius, /*MaterialId=*/0); // MaterialId set to dirt material
        
                // Debug draw
                DrawDebugSphere(GetWorld(), Location, 20.0f, 12, FColor::Red, false, 5.0f, 0, 1.0f);
            }
}
