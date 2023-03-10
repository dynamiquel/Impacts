// © 2021 Vixentail, Liam Hall and Savir Sohal


#include "BasicFootstepsSubsystem.h"

#include "BFSurfaceTypeMaterials.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetSystemLibrary.h"

void UBasicFootstepsSubsystem::PostInitialize()
{
	Super::PostInitialize();

	if (!GetWorld()->IsGameWorld())
		return;
	
	InitialiseMap();
	InitialiseSoundConcurrency();
}

EBFSurfaceType UBasicFootstepsSubsystem::GetMaterialSurfaceType(const UPhysicalMaterial* PhysicsMaterial)
{
	if (!PhysicsMaterial)
		return EBFSurfaceType::None;
	
	return MaterialsSurfaceType.FindRef(PhysicsMaterial);
}

void UBasicFootstepsSubsystem::InitialiseMap()
{
	FString DTPath = TEXT("DataTable'/BasicFootsteps/DataTables/DT_Materials.DT_Materials'");
	
	const UDataTable* SurfacesDT = (
		LoadObject<UDataTable>(nullptr, *DTPath));

	checkf(SurfacesDT, TEXT("Could not find Materials DataTable at %s"), *DTPath);

	MaterialsSurfaceType.Reserve(SurfacesDT->GetRowMap().Num());

	SurfacesDT->ForeachRow<FBFSurfaceTypeMaterials>(TEXT("Materials"), [this](
	const FName& Key, const FBFSurfaceTypeMaterials& Value)
	{
		for (auto PhysicsMaterial : Value.PhysicsMaterials)
			MaterialsSurfaceType.Add(PhysicsMaterial, Value.SurfaceType);
	});
}

void UBasicFootstepsSubsystem::InitialiseSoundConcurrency()
{
	// Prevents too many footstep/impact sounds from playing at once. Engine can only handle so many sounds.
	SoundConcurrency = NewObject<USoundConcurrency>(
		this,
		USoundConcurrency::StaticClass(),
		FName(TEXT("BFSoundConcurrency")));
	
	checkf(SoundConcurrency, TEXT("UBasicFootstepsSubsystem: Could not create Sound Concurrency settings."));
	
	SoundConcurrency->Concurrency.MaxCount = 8;
	SoundConcurrency->Concurrency.bLimitToOwner = false;
	SoundConcurrency->Concurrency.ResolutionRule = EMaxConcurrentResolutionRule::StopFarthestThenOldest;
	SoundConcurrency->Concurrency.RetriggerTime = .1f;
}
