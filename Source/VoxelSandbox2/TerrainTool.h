#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerrainTool.generated.h"

class ASandboxTerrainController;

UENUM(BlueprintType)
enum class ShapeType : uint8
{
    Circle UMETA(DisplayName = "Circle"),
    Cylinder UMETA(DisplayName = "Cylinder"),
    Cube UMETA(DisplayName = "Cube")
};

UCLASS()
class VOXELSANDBOX2_API ATerrainTool : public AActor
{
    GENERATED_BODY()

public:
    ATerrainTool();

    // Main dig and fill methods
    virtual void DigTerrain(const FHitResult& HitResult);
    virtual void FillTerrain(const FHitResult& HitResult);

    // Sets the shape and size parameters
    void SetShape(ShapeType NewShape);
    void SetSize(float NewSize);

protected:  // Change to protected to allow derived classes access
    virtual void PerformDigAction(const FVector& Location);  // Keep these as protected
    virtual void PerformFillAction(const FVector& Location);  // Keep these as protected

    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tool")
    float DigFillRadius = 50.0f;  // Default dig radius

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tool")
    int MaterialId = 0;  // Default material ID for filling

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tool")
    ShapeType CurrentShape = ShapeType::Circle;

    ASandboxTerrainController* TerrainController;
};
