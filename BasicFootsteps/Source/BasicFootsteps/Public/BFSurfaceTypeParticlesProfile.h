#pragma once

#include "BFSurfaceType.h"
#include "NiagaraSystem.h"
#include "BFSurfaceTypeParticlesProfile.generated.h"

UCLASS(BlueprintType)
class UBFSurfaceTypeParticlesProfile : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* DefaultParticles = nullptr;
	
	UPROPERTY(EditAnywhere)
	TMap<EBFSurfaceType, UNiagaraSystem*> SurfaceParticles;

	UNiagaraSystem* Find(EBFSurfaceType SurfaceType)
	{
		if (UNiagaraSystem** ParticleSystem = SurfaceParticles.Find(SurfaceType))
			return *ParticleSystem;
	
		return DefaultParticles;
	}
};
