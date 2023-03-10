// © 2021 Vixentail, Liam Hall and Savir Sohal

#pragma once

#include "CoreMinimal.h"
#include "BFSurfaceType.h"
#include "BFSurfaceTypeParticlesProfile.h"
#include "BFSurfaceTypeSoundsProfile.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Components/AudioComponent.h"
#include "BFFootstepsComponent.generated.h"

USTRUCT(BlueprintType)
struct FBFFoot
{
	GENERATED_BODY()
	
	/**
	 * @brief Using Components requires more setup as you need to create and assign the Components yourself,
	 * however, they are more performant.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseComponents = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="!bUseComponents", EditConditionHides))
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="!bUseComponents", EditConditionHides))
	float VolumeMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="!bUseComponents", EditConditionHides))
	USkeletalMeshComponent* CharacterMeshComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bUseComponents", EditConditionHides))
	UAudioComponent* AudioComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bUseComponents", EditConditionHides))
	UNiagaraComponent* ParticleSystemComponent = nullptr;
};

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BASICFOOTSTEPS_API UBFFootstepsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBFFootstepsComponent();
	
	/**
	 * @brief 
	 * @param SurfaceType The Surface Type underneath the Foot.
	 * @param Speed The current speed of the Actor.
	 * @param FootIndex The Foot to use (usually, 0 = left, 1 = right).
	 */
	virtual void Play(EBFSurfaceType SurfaceType, float Speed, int32 FootIndex = 0);
	
	/**
	 * @brief 
	 * @param PhysicalMaterial The Physical Material underneath the Foot.
	 * @param Speed The current speed of the Actor.
	 * @param FootIndex The Foot to use (usually, 0 = left, 1 = right).
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void Play(UPhysicalMaterial* PhysicalMaterial, float Speed, int32 FootIndex = 0);

	/**
	 * @brief Similar to Play but also calculates the Physical Material of the Mesh by using the stored Foot Socket
	 * location.
	 *
	 * If the results are not desired, you can:
	 *
	 * 1) Change the TraceStartOffset and TraceEndOffset variables.
	 *
	 * 2) Override this function.
	 *
	 * 3) Calculate the Physical Material yourself and use the Play() method.
	 * 
	 * Note: Only works when the Foot is using Sockets as opposed to Components.
	 * @param Speed The current speed of the Actor.
	 * @param FootIndex The Foot to use (usually, 0 = left, 1 = right).
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void CalculateAndPlay(float Speed, int32 FootIndex = 0);

protected:
	// SOUNDS
	/**
	 * @brief Enable footstep sounds.
	 */
	UPROPERTY(EditAnywhere)
	bool bEnableFootstepSounds = true;

	/**
	 * @brief The Sound Profile these footsteps should use.
	 *
	 * Sound Profile determines which Sounds should be played
	 * depending on the Surface Type of the Physical Material of the Mesh.
	 */
	UPROPERTY(EditAnywhere, meta=(EditCondition="bEnableFootstepSounds", EditConditionHides))
	UBFSurfaceTypeSoundsProfile* SoundsProfile = nullptr;
	
	/**
	 * @brief How much the volume of the footstep sound is affected by the Actor's speed. 1 = normal volume.
	 */
	UPROPERTY(EditAnywhere, meta=(EditCondition="bEnableFootstepSounds", EditConditionHides))
	FRuntimeFloatCurve SpeedToVolumeMultiplier;

	// PARTICLES
	/**
	 * @brief Enable footstep particles.
	 */
	UPROPERTY(EditAnywhere)
	bool bEnableFootstepParticles = false;

	/**
	 * @brief The Particles Profile these footsteps should use.
	 *
	 * Particles Profile determines which Particle Effects should be played
	 * depending on the Surface Type of the Physical Material of the Mesh.
	 */
	UPROPERTY(EditAnywhere, meta=(EditCondition="bEnableFootstepParticles", EditConditionHides))
	UBFSurfaceTypeParticlesProfile* ParticlesProfile = nullptr;
	
	
	UPROPERTY(VisibleInstanceOnly, AdvancedDisplay)
	TArray<FBFFoot> Feet;

	/**
	 * @brief The minimum number of seconds that must pass before playing another footstep.
	 */
	UPROPERTY(EditAnywhere, AdvancedDisplay, meta=(ClampMax=2.f, ClampMin=0.f))
	float MinFootstepInterval = 0.1f;
	
	/**
	 * @brief Where the trace should start from the Socket Location.
	 *
	 * May want to increase this for water depth reasons.
	 * 
	 * Only applies when using Sockets and the CalculateAndPlay method.
	 */
	UPROPERTY(EditAnywhere, AdvancedDisplay, meta=(ClampMax=100.f, ClampMin=0.f))
	float TraceStartOffset = 25.f;

	/**
	 * @brief Where the trace should end from the Socket Location.
	 * 
	 * Only applies when using Sockets and the CalculateAndPlay method.
	 */
	UPROPERTY(EditAnywhere, AdvancedDisplay, meta=(ClampMax=0.f, ClampMin=-50.f))
	float TraceEndOffset = -25.f;

	FTimerHandle FootstepIntervalTimer;
	
public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	const TArray<FBFFoot>& GetFeet() const { return Feet; }
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	USoundBase* GetSoundForSurfaceType(EBFSurfaceType Surface);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	UNiagaraSystem* GetParticlesForSurfaceType(EBFSurfaceType Surface);

	int32 AddFoot(FBFFoot Foot);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	int32 AddFootUsingComponents(UAudioComponent* AudioComponent, UNiagaraComponent* ParticleSystem);

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, DisplayName="Add Foot Using Socket")
	int32 AddFootUsingSocket(USkeletalMeshComponent* SkeletalMeshComponent, FName SocketName, float VolumeMultiplier = 1.f);
};