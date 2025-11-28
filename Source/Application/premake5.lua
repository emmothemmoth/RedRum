include "../../Premake/extensions.lua"
-- include for common stuff 
include "../../Premake/common.lua"
-------------------------------------------------------------
project "Application"
location (dirs.projectfiles)
dependson { "Editor"}

kind "WindowedApp"
language "C++"
cppdialect "C++20"

targetdir (dirs.bin)
targetname("%{prj.name}_%{cfg.buildcfg}")
objdir (dirs.intermidiate .. "%{prj.name}/%{cfg.buildcfg}")

links ({"Editor"})
print(dirs.game)
includedirs {dirs.application, dirs.utilities}

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"*.aps",
		"*.rc"
	}
	
	filter "action:vs*"
	files {"Resources/**"}
	buildaction "ResourceCompile"

	libdirs { dirs.lib, dirs.engineexternal.."FBXImporter/lib/%{cfg.buildcfg}/"}
	
	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
		targetname("RedRum_Debug")
		flags { 
			"FatalCompileWarnings" 	
		}

	filter "configurations:Release"
		defines {"_RELEASE", "NDEBUG"}
		runtime "Release"
		optimize "on"
		targetname("RedRum_Release")

		flags { 
			"FatalCompileWarnings" 	
		}
	filter "configurations:Retail"
		defines {"_RETAIL", "NDEBUG"}
		runtime "Release"
		targetname("RedRum")

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
		links({"d3d11","dxguid.lib", "TGAFbx.lib"})
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			-- "FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		