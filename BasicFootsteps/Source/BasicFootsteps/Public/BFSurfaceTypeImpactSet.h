#pragma once

#include "BFSurfaceTypeImpactSet.generated.h"

class UNiagaraSystem;
USTRUCT(BlueprintType)
struct FBFSurfaceTypeImpactSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	USoundBase* Sound = nullptr;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* Particles = nullptr;
};
