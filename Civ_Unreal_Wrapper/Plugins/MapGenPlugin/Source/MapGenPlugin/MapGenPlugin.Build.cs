// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class MapGenPlugin : ModuleRules
{
	public MapGenPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		// Path to the StrategyMapGenerationPlugin source
		string MapGenCorePath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../../../StrategyMapGenerationPlugin"));
		string MapGenCoreIncludePath = Path.Combine(MapGenCorePath, "include");
		string MapGenCoreSrcPath = Path.Combine(MapGenCorePath, "src");
		
		PublicIncludePaths.AddRange(
			new string[] {
				MapGenCoreIncludePath
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				MapGenCoreSrcPath
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
		
		// Add all the C++ source files from StrategyMapGenerationPlugin
		PublicAdditionalLibraries.AddRange(
			new string[]
			{
			}
			);
		
		// Define MAPGEN_EXPORTS for building the library
		PublicDefinitions.Add("MAPGEN_EXPORTS=1");
		
		// Disable PCH for third-party code
		bUseUnity = false;

        //Normalize floating point precision between platforms
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PrivateAdditionalCompilerArguments += " /fp:precise";
		}
		else
		{
			PrivateAdditionalCompilerArguments += " -ffp-contract=off";
		}
	}
}
