// © 2021 Vixentail, Liam Hall and Savir Sohal

#pragma once

#include "CoreMinimal.h"
#include "BFSurfaceTypeImpactSet.h"
#include "Components/ActorComponent.h"
#include "BFImpactorComponent.generated.h"


class UNiagaraSystem;
enum class EBFSurfaceType : uint8;
class UBFSurfaceTypeImpactsProfile;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BASICFOOTSTEPS_API UBFImpactorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBFImpactorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void CreateImpact(const UPhysicalMaterial* PhysicalMaterial, const FVector& ImpactLocation, const FRotator& ImpactRotation, const float Speed);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void CreateImpact_Normal(const UPhysicalMaterial* PhysicalMaterial, const FVector& ImpactLocation, const FVector& ImpactNormal, const float Speed);
	
	
protected:
	UPROPERTY(EditAnywhere)
	bool bEnableImpactSounds = false;

	UPROPERTY(EditAnywhere, meta=(EditCondition="bEnableImpactSounds", EditConditionHides))
	FRuntimeFloatCurve SpeedToVolumeMultiplier;
	
	UPROPERTY(EditAnywhere)
	bool bEnableImpactParticles = false;

	UPROPERTY(EditAnywhere, meta=(EditCondition="bEnableImpactParticles", EditConditionHides))
	FRuntimeFloatCurve SpeedToParticleSizeMultiplier;

	/**
	 * @brief The Impact Profile this object should use.
	 *
	 * Impact Profile determines which Sound and Particle Effects should be played
	 * depending on the Surface Type of the Physical Material of the Mesh.
	 */
	UPROPERTY(EditAnywhere)
	UBFSurfaceTypeImpactsProfile* ImpactsProfile = nullptr;

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	const FBFSurfaceTypeImpactSet& GetImpactSetForSurfaceType(EBFSurfaceType Surface);
};
