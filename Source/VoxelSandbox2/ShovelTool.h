#pragma once

#include "CoreMinimal.h"
#include "TerrainTool.h"
#include "ShovelTool.generated.h"

UCLASS()
class VOXELSANDBOX2_API AShovelTool : public ATerrainTool
{
    GENERATED_BODY()

public:
    AShovelTool();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ShovelMesh;

protected:
    virtual void PerformDigAction(const FVector& Location) override;
    virtual void PerformFillAction(const FVector& Location) override;
};