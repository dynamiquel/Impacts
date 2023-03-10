// © 2021 Vixentail, Liam Hall and Savir Sohal


#include "BFImpactorComponent.h"

#include "BasicFootstepsSubsystem.h"
#include "BFSurfaceTypeImpactsProfile.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UBFImpactorComponent::UBFImpactorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	FString IPPath = TEXT("BFSurfaceTypeImpactsProfile'/BasicFootsteps/DataAssets/ImpactProfiles/IP_NormalBullet.IP_NormalBullet'");
	ImpactsProfile = LoadObject<UBFSurfaceTypeImpactsProfile>(nullptr, *IPPath);

	SpeedToVolumeMultiplier.EditorCurveData.AddKey(0, 1);
	SpeedToParticleSizeMultiplier.EditorCurveData.AddKey(0, 1);
}


// Called when the game starts
void UBFImpactorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UBFImpactorComponent::CreateImpact(const UPhysicalMaterial* PhysicalMaterial, const FVector& ImpactLocation,
	const FRotator& ImpactRotation, const float Speed)
{
	if (IsRunningDedicatedServer())
		return;

	const EBFSurfaceType SurfaceType =
		GetWorld()->GetSubsystem<UBasicFootstepsSubsystem>()->GetMaterialSurfaceType(PhysicalMaterial);

	const FBFSurfaceTypeImpactSet& ImpactSet = GetImpactSetForSurfaceType(SurfaceType);

	if (bEnableImpactSounds && ImpactSet.Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(
				this,
				ImpactSet.Sound,
				ImpactLocation,
				SpeedToVolumeMultiplier.GetRichCurveConst()->Eval(Speed, 1),
				1, 0, nullptr,
				GetWorld()->GetSubsystem<UBasicFootstepsSubsystem>()->GetSoundConcurrency());
	}

	if (bEnableImpactParticles && ImpactSet.Particles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				this,
				ImpactSet.Particles,
				ImpactLocation,
				ImpactRotation,
				FVector(SpeedToParticleSizeMultiplier.GetRichCurveConst()->Eval(Speed, 1)),
				true,
				true,
				ENCPoolMethod::AutoRelease);
	}
}

void UBFImpactorComponent::CreateImpact_Normal(const UPhysicalMaterial* PhysicalMaterial, const FVector& ImpactLocation,
	const FVector& ImpactNormal, const float Speed)
{
	// Convert Location and Normal to an accurate rotation.
	FVector Right = GetOwner()->GetActorUpVector().Cross(ImpactNormal);
	Right.Normalize();
	const FVector Forward = Right.Cross(ImpactNormal);
	const FRotator ImpactRotation = UKismetMathLibrary::MakeRotationFromAxes(Forward, Right, ImpactNormal);
	
	CreateImpact(PhysicalMaterial, ImpactLocation, ImpactRotation, Speed);
}

const FBFSurfaceTypeImpactSet& UBFImpactorComponent::GetImpactSetForSurfaceType(EBFSurfaceType Surface)
{
	checkf(ImpactsProfile,
		TEXT("You have not selected an Impact Profile to use."),
		*GetOwner()->GetName());
	
	return ImpactsProfile->Find(Surface);
}
