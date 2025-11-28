print("DearImGui")
include "../....//../Premake/common.lua"
-------------------------------------------------------------
project "DearImGui"
location (dirs.projectfiles)

kind "StaticLib"
language "C++"
cppdialect "C++20"
debugdir ("%{dirs.intermidiate}/%{prj.name}/")
targetdir (dirs.lib.."/%{cfg.buildcfg}")
targetname("%{prj.name}")



	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"*.aps",
		"*.rc"
	}

	libdirs { dirs.lib }
	
	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
				warnings "Off"
	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"
				warnings "Off"
		flags { 
			"FatalCompileWarnings"
		}
	filter "configurations:Retail"
		defines "_RETAIL"
		runtime "Release"
		optimize "on"
				warnings "Off"
		flags { 
			"FatalCompileWarnings"
		}

	filter "system:windows"
--		kind "StaticLib"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Off"
		--conformanceMode "On"
		--buildoptions { "/permissive" }
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			-- "FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		