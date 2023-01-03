#pragma once

#include "BFSurfaceType.h"
#include "Engine/DataTable.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "BFSurfaceTypeMaterials.generated.h"

USTRUCT(BlueprintType)
struct FBFSurfaceTypeMaterials : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<UPhysicalMaterial*> PhysicsMaterials;

	UPROPERTY(EditAnywhere)
	EBFSurfaceType SurfaceType = EBFSurfaceType::None;
};
