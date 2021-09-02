// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
using System.IO;
using UnrealBuildTool;

public class LuaProvider : ModuleRules
{
	public LuaProvider(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// Add the import library
			PublicLibraryPaths.Add(Path.Combine(ModuleDirectory, "../../Content","Lib", "Win64", "Release"));
			PublicAdditionalLibraries.Add("lua53.lib");
            // Delay-load the DLL, so we can load it from the right place first
            PublicDelayLoadDLLs.Add("lua53.dll");
        }
        else if (Target.Platform == UnrealTargetPlatform.Android)
        {
            PublicLibraryPaths.Add(Path.Combine(ModuleDirectory,"../../Content", "Lib", "Android", "armeabi-v7a"));
            PublicAdditionalLibraries.Add("lua");
            string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target/*BuildConfiguration*/.RelativeEnginePath);
            AdditionalPropertiesForReceipt.Add(new ReceiptProperty("AndroidPlugin", Path.Combine(PluginPath, "LuaProvider_APL.xml")));
        }
		
		PublicIncludePaths.AddRange(
			new string[] {
				"Runtime/Core/Public/Modules",
				"Runtime/Projects/Public/Interfaces",
				ModuleDirectory + "/Public",
				ModuleDirectory + "/Classes"
				// ... add public include paths required here ...
			}
			);
		PrivateIncludePaths.AddRange(
			new string[] {
				ModuleDirectory + "/Private",
				ModuleDirectory + "/Lua/Include"
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Projects",
                "Json"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "Engine",
                "InputCore"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
