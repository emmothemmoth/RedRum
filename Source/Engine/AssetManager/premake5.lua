print("AssetManager")
include "../../../Premake/common.lua"
-------------------------------------------------------------
project "AssetManager"
location (dirs.projectfiles)

kind "StaticLib"
language "C++"
cppdialect "C++20"
debugdir ("%{dirs.intermidiate}/%{prj.name}/")
targetdir (dirs.lib.."/%{cfg.buildcfg}")
targetname("%{prj.name}")

print(dirs.assetmanager)
includedirs {dirs.assetmanager, dirs.utilities, dirs.engineexternal .."FBXImporter/include/",dirs.graphicsengine.."Objects/"}

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"*.aps",
		"*.rc"
	}

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"*.aps",
		"*.rc"
	}

	libdirs { dirs.lib, dirs.engineexternal.."FBXImporter/lib/%{cfg.buildcfg}/"}
	
	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"
		flags { 
			"FatalCompileWarnings"
		}
	filter "configurations:Retail"
		defines "_RETAIL"
		runtime "Release"
		optimize "on"
		flags { 
			"FatalCompileWarnings"
		}

	filter "system:windows"
		kind "StaticLib"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"
		links{"TGAFbx.lib"}
		flags { 
			"MultiProcessorCompile"
		}
		