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

    CurrentShape = ShapeType::Cube;
    // You can customize shovel-specific settings here (e.g., DigFillRadius)
    DigFillRadius = 50.0f; // Example size for a shovel
}
