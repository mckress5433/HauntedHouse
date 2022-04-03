// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HauntedHouse : ModuleRules
{
	public HauntedHouse(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
