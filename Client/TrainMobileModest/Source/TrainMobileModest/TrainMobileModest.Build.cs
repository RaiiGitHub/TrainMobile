// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class TrainMobileModest : ModuleRules
{
    public TrainMobileModest(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore","UMG", "AIModule" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "."));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Widgets"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Tool"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Fx"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "../../Plugins/LuaProvider/Source/LuaProvider/Lua/Include"));

        // Uncomment if you are using Slate UI
        PrivateDependencyModuleNames.AddRange(new string[] 
        { "Slate",
          "SlateCore",
          "MoviePlayer",
          "LuaProvider",
          "TrainOnline",      
          "WebBrowser"
        });

        if ((Target.Platform == UnrealTargetPlatform.Android))
            PrivateDependencyModuleNames.AddRange(new string[] { "AndroidMoviePlayer"});
}
}
