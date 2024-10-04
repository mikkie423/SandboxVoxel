// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VoxelSandbox2 : ModuleRules
{
	public VoxelSandbox2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UnrealSandboxTerrain" });
	}
}
