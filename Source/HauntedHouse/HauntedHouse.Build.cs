// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HauntedHouse : ModuleRules
{
	public HauntedHouse(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"OnlineSubsystemSteam", 
			"OnlineSubsystem",
			"OnlineSubsystemUtils"
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks", 
			"ModelViewViewModel"
		});
	}
}
