include "../Premake/extensions.lua"
-- include for common stuff 
include "../Premake/common.lua"

workspace "RedRum"
location "../"
startproject "Application"
architecture "x64"

configurations {
	"Debug",
	"Release",
	"Retail"
}
 print("Application")
 include(dirs.application)

print("Engine")
group "Engine"
include(dirs.assetmanager)
include(dirs.gameEngine)
include(dirs.graphicsengine)
include(dirs.shaders)

print("Editor")
include(dirs.editor)

