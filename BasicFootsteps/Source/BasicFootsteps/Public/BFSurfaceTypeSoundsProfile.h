#pragma once

#include "BFSurfaceType.h"
#include "BFSurfaceTypeSoundsProfile.generated.h"


UCLASS(BlueprintType)
class UBFSurfaceTypeSoundsProfile : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	USoundBase* DefaultSound = nullptr;
	
	UPROPERTY(EditAnywhere)
	TMap<EBFSurfaceType, USoundBase*> SurfaceSounds;

	USoundBase* Find(EBFSurfaceType SurfaceType)
	{
		if (USoundBase** Sound = SurfaceSounds.Find(SurfaceType))
			return *Sound;
	
		return DefaultSound;
	}
};
