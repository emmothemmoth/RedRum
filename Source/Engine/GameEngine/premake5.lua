include "../../../Premake/extensions.lua"
-- include for common stuff 
include "../../../Premake/common.lua"

 group "Engine"
-------------------------------------------------------------
project "GameEngine"
location (dirs.projectfiles)
dependson { "GraphicsEngine, CommonUtilities, Logger"}

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
links ({"GraphicsEngine","CommonUtilities", "Logger"})
print(dirs.gameEngine)
includedirs {dirs.gameEngine, dirs.utilities}

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
		--buildoptions { "/permissive" }
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			-- "FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		