// © 2021 Vixentail, Liam Hall and Savir Sohal


#include "BFFootstepsComponent.h"

#include "BasicFootstepsSubsystem.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

UBFFootstepsComponent::UBFFootstepsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	FString SPPath = TEXT("BFSurfaceTypeSoundsProfile'/BasicFootsteps/DataAssets/SoundProfiles/SP_Humanoid.SP_Humanoid'");
	SoundsProfile = LoadObject<UBFSurfaceTypeSoundsProfile>(nullptr, *SPPath);

	FString PPPath = TEXT("BFSurfaceTypeSoundsProfile'/BasicFootsteps/DataAssets/ParticlesProfiles/PP_Humanoid.PP_Humanoid'");
	ParticlesProfile = LoadObject<UBFSurfaceTypeParticlesProfile>(nullptr, *PPPath);

	SpeedToVolumeMultiplier.EditorCurveData.AddKey(0, 1);
}

void UBFFootstepsComponent::Play(EBFSurfaceType SurfaceType, float Speed, int32 FootIndex)
{
	if (IsRunningDedicatedServer())
		return;
	
	if (FootIndex < 0 || Feet.Num() == 0)
		return;
	
	// Min footstep interval hasn't been reached.
	if (MinFootstepInterval > 0 && GetWorld()->GetTimerManager().IsTimerActive(FootstepIntervalTimer))
		return;

	// If using a foot index higher than the feet, just cycle through the feet.
	auto Foot = Feet[FootIndex % Feet.Num()];
	
	if (Foot.bUseComponents)
	{
		if (bEnableFootstepSounds)
		{
			const auto AudioComp = Foot.AudioComponent;
			if (ensureAlwaysMsgf(IsValid(AudioComp),
				TEXT("Foot Audio Component for %s is invalid"), *GetOwner()->GetName()))
			{
				const auto Sound = GetSoundForSurfaceType(SurfaceType);

				AudioComp->SetSound(Sound);
				AudioComp->Play();
			}
		}

		if (bEnableFootstepParticles)
		{
			const auto ParticleSystem = Foot.ParticleSystemComponent;
			if (ensureAlwaysMsgf(IsValid(ParticleSystem),
				TEXT("Foot Particle System Component for %s is invalid"), *GetOwner()->GetName()))
			{
				const auto Particles = GetParticlesForSurfaceType(SurfaceType);

				ParticleSystem->SetAsset(Particles);
				ParticleSystem->ActivateSystem();
			}
		}
	}
	else
	{
		const auto MeshComp = Foot.CharacterMeshComponent;
		checkf(IsValid(MeshComp),
			TEXT("The specified Character Mesh for %s is invalid"), *GetOwner()->GetName());

		const FTransform SocketTransform = MeshComp->GetSocketTransform(Foot.SocketName);
		checkf(!SocketTransform.GetLocation().IsZero(), TEXT("The specified Socket '%s' for %s doesn't exist."), *Foot.SocketName.ToString(), *GetOwner()->GetName());

		if (bEnableFootstepSounds)
		{
			const auto Sound = GetSoundForSurfaceType(SurfaceType);
			
			// Footstep sound.
			UGameplayStatics::PlaySoundAtLocation(
				this,
				Sound,
				SocketTransform.GetLocation(),
				Foot.VolumeMultiplier *
				SpeedToVolumeMultiplier.GetRichCurveConst()->Eval(Speed, 1),
				1.f, 0.f, nullptr,
				GetWorld()->GetSubsystem<UBasicFootstepsSubsystem>()->GetSoundConcurrency());
		}

		if (bEnableFootstepParticles)
		{
			const auto Particles = GetParticlesForSurfaceType(SurfaceType);

			// Since footprints don't use impact location, the footprint will not be perfect.

			// Add a small amount of height to reduce chance of footprint being in the ground.
			FVector Location = SocketTransform.GetLocation();
			Location.Z += 3.f;

			// Remove pitch from the footstep rotation, otherwise, it won't be a perfect 'footprint'.
			FRotator Rotation = SocketTransform.Rotator();
			Rotation.Pitch = 0;

			// Footstep particles.
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				this,
				Particles,
				Location,
				Rotation,
				SocketTransform.GetScale3D(),
				true,
				true,
				ENCPoolMethod::AutoRelease);
		}
	}

	// Delay to prevent footsteps getting triggered too quickly.
	GetWorld()->GetTimerManager().SetTimer(FootstepIntervalTimer, MinFootstepInterval, false);
}

void UBFFootstepsComponent::Play(UPhysicalMaterial* PhysicalMaterial, float Speed, int32 FootIndex)
{
	const EBFSurfaceType SurfaceType =
		GetWorld()->GetSubsystem<UBasicFootstepsSubsystem>()->GetMaterialSurfaceType(PhysicalMaterial);

	Play(SurfaceType, Speed, FootIndex);
}

void UBFFootstepsComponent::CalculateAndPlay(float Speed, int32 FootIndex)
{
	if (IsRunningDedicatedServer())
		return;
	
	if (FootIndex < 0 || Feet.Num() == 0)
		return;
	
	auto Foot = Feet[FootIndex % Feet.Num()];

	const auto MeshComp = Foot.CharacterMeshComponent;
	checkf(IsValid(MeshComp),
		TEXT("The specified Character Mesh for %s is invalid"), *GetOwner()->GetName());

	FVector FootLocation = MeshComp->GetSocketLocation(Foot.SocketName);
	FVector StartLocation = FootLocation + FVector(0, 0, TraceStartOffset);
	FVector EndLocation = FootLocation + FVector(0, 0, TraceEndOffset);

	FCollisionQueryParams QueryParams = FCollisionQueryParams::DefaultQueryParam;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.bReturnPhysicalMaterial = true;
	
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		QueryParams);

	// Likely in air.
	if (!HitResult.bBlockingHit)
		return;

	Play(HitResult.PhysMaterial.Get(), Speed, FootIndex);
}

USoundBase* UBFFootstepsComponent::GetSoundForSurfaceType(EBFSurfaceType Surface)
{
	checkf(SoundsProfile,
		TEXT("You have enabled Footstep Sounds for %s but have not specified a Sounds Profile to use."),
		*GetOwner()->GetName());
	
	return SoundsProfile->Find(Surface);
}

UNiagaraSystem* UBFFootstepsComponent::GetParticlesForSurfaceType(EBFSurfaceType Surface)
{
	checkf(ParticlesProfile,
		TEXT("You have enabled Footstep Particles for %s but have not specified a Particles Profile to use."),
		*GetOwner()->GetName());
	
	return ParticlesProfile->Find(Surface);
}

int32 UBFFootstepsComponent::AddFoot(FBFFoot Foot)
{
	return Feet.Add(Foot);
}

int32 UBFFootstepsComponent::AddFootUsingComponents(UAudioComponent* AudioComponent, UNiagaraComponent* ParticleSystem)
{
	FBFFoot Foot;
	Foot.bUseComponents = true;
	Foot.AudioComponent = AudioComponent;
	Foot.ParticleSystemComponent = ParticleSystem;

	return AddFoot(Foot);
}

int32 UBFFootstepsComponent::AddFootUsingSocket(USkeletalMeshComponent* SkeletalMeshComponent, FName SocketName,
	float VolumeMultiplier)
{
	FBFFoot Foot;
	Foot.bUseComponents = false;
	Foot.CharacterMeshComponent = SkeletalMeshComponent;
	Foot.SocketName = SocketName;
	Foot.VolumeMultiplier = VolumeMultiplier;

	return AddFoot(Foot);	
}

