// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class dazAda : ModuleRules
{
	public dazAda(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayAbilities", "GameplayTags", "GameplayTasks","StateTreeEditorModule" });
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"Niagara",
			"ModularGameplay",
			"GameFeatures",
			"GameplayStateTreeModule",
			"StateTreeModule",
			"AIModule",
			"NavigationSystem",
		});
	}
}
