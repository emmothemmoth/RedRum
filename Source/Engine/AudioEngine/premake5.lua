print("AudioEngine")
include "../../../Premake/extensions.lua"
-- include for common stuff 
include "../../../Premake/common.lua"

 group "Engine"
-------------------------------------------------------------
project "AudioEngine"
location (dirs.projectfiles)
dependson { "CommonUtilities, Logger"}

kind "StaticLib"
language "C++"
cppdialect "C++20"

debugdir ("%{dirs.intermidiate}/%{prj.name}/")
targetdir (dirs.lib.."/%{cfg.buildcfg}")
targetname("%{prj.name}")
objdir (dirs.intermidiate .. "%{prj.name}/%{cfg.buildcfg}")

pchheader ("%{prj.name}.pch.h")
pchsource ("%{prj.name}.pch.cpp")
forceincludes ("%{prj.name}.pch.h")
--postbuildcommands {"\"$(VC_ExecutablePath_x64)\\lib.exe\" /out:\"$(SolutionDir)Lib/%{cfg.buildcfg}/%{prj.name}.lib\" \"$(SolutionDir)/Intermediate/%{prj.name}/%{cfg.buildcfg}*.obj\""}
-- pchheader ("%{prj.name}.pch.h")
-- pchsource ("%{prj.name}.pch.cpp")
-- forceincludes ("%{prj.name}.pch.h")
defines {
	"JUCE_APP_CONFIG_HEADER=\"AppConfig.h\"",
        "JUCE_DONT_DECLARE_PROJECTINFO=1",
        "DONT_SET_USING_JUCE_NAMESPACE=1"
}
links ({"CommonUtilities", "Logger"})
includedirs {dirs.utilities, dirs.juceModules, dirs.juceConfig}

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"*.aps",
		"*.rc"
	}
	
	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
		flags { 
			"FatalCompileWarnings" 	
		}
	filter "configurations:Release"
		defines {"_RELEASE", "NDEBUG"}
		runtime "Release"
		optimize "on"
		flags { 
			"FatalCompileWarnings" 	
		}
	filter "configurations:Retail"
		defines {"_RETAIL", "NDEBUG"}
		runtime "Release"
		optimize "on"
		flags { 
			"FatalCompileWarnings"
		}

	filter "system:windows"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"
		--links{"d3d11","dxguid", "dxgi", "d3dcompiler" }
		--conformanceMode "On"
		buildoptions { "/bigobj" }
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			-- "FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		