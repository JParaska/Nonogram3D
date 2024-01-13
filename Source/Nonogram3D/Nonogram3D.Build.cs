// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class Nonogram3D : ModuleRules
{
	public Nonogram3D(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "EngineSettings", "UMG" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        PublicIncludePaths.AddRange(
            new string[] {
                "Nonogram3D",

                "Nonogram3D/Gameplay",

                "Nonogram3D/Player",
				
				"Nonogram3D/Subsystem",
                "Nonogram3D/Subsystem/SaveGame",
				
				"Nonogram3D/Global"
        });
    }
}
