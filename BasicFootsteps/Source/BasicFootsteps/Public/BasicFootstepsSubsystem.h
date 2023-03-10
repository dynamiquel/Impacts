// © 2021 Vixentail, Liam Hall and Savir Sohal

#pragma once

#include "CoreMinimal.h"
#include "BFSurfaceType.h"
#include "Subsystems/WorldSubsystem.h"
#include "BasicFootstepsSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class BASICFOOTSTEPS_API UBasicFootstepsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void PostInitialize() override;

protected:
	UPROPERTY()
	TMap<const UPhysicalMaterial*, EBFSurfaceType> MaterialsSurfaceType;

	UPROPERTY()
	USoundConcurrency* SoundConcurrency;

public:
	UFUNCTION(BlueprintCallable, Category="Footsteps")
	FORCEINLINE EBFSurfaceType GetMaterialSurfaceType(const UPhysicalMaterial* PhysicsMaterial);

	USoundConcurrency* GetSoundConcurrency() const { return SoundConcurrency; }

protected:
	void InitialiseMap();

	void InitialiseSoundConcurrency();
};
