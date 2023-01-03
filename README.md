# Impacts
Impacts is a basic hit impact and footsteps system for Unreal Engine 5, which uses physical materials to determine which sounds and Niagara particle effects to apply.

## How-to-use
### Footstep sound effects
To enable footstep sound effects, you first need to create a SoundProfile DataAsset. We recommend making a SoundProfile for every species type. A Humanoid one is already provided.
Within this SoundProfile, you can specify which sounds to play depending on the surface type.

### Registering surface types
First things first, you need to register your Physics Materials to a corresponding SurfaceType. This can be done in the DT_Materials database provided within the Plugin. A few examples have already been provided. To create more SurfaceTypes, you need to edit the BFSurfaceType.h file.

### Footstep particle effects
Similarly to sound effects, you need to create a ParticlesProfile DataAsset.

### Hit impacts
For hit impacts, i.e. bullet impacts, you need to create an ImpactProfile DataAsset.


### Where's the sounds and particles?
This plugin does not contain any audio or particle files. You are expected to provide your own or from a third-party resource.

### Why is it called BasicFootsteps?
When I created the plugin, it was originally intended for the use of footsteps only but naturally included hit effects too as they are basically the same thing.