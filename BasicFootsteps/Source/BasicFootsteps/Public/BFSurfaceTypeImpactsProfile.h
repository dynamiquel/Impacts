#pragma once

#include "BFSurfaceType.h"
#include "BFSurfaceTypeImpactSet.h"
#include "BFSurfaceTypeImpactsProfile.generated.h"


UCLASS(BlueprintType)
class UBFSurfaceTypeImpactsProfile : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FBFSurfaceTypeImpactSet DefaultSet;
	
	UPROPERTY(EditAnywhere)
	TMap<EBFSurfaceType, FBFSurfaceTypeImpactSet> SurfaceSets;

	const FBFSurfaceTypeImpactSet& Find(EBFSurfaceType SurfaceType)
	{
		if (const FBFSurfaceTypeImpactSet* Set = SurfaceSets.Find(SurfaceType))
			return *Set;
	
		return DefaultSet;
	}
};
