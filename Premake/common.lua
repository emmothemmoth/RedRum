----------------------------------------------------------------------------
-- the dirs table is a listing of absolute paths, since we generate projects
-- and files it makes a lot of sense to make them absolute to avoid problems
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
dirs = {}
dirs["root"] 			= os.realpath("../")
dirs["bin"]				= os.realpath(dirs.root .. "Bin/")
dirs["temp"]			= os.realpath(dirs.root .. "Temp/")
dirs["lib"]				= os.realpath(dirs.root .. "Lib/")
dirs["projectfiles"]	= os.realpath(dirs.root .. "Local/")
dirs["source"] 			= os.realpath(dirs.root .. "Source/")
dirs["intermidiate"] 	= os.realpath(dirs.root .. "Intermediate/")
dirs["application"] 	= os.realpath(dirs.root .. "Source/Application/")
dirs["editor"] 	        = os.realpath(dirs.root .. "Source/Editor/")
dirs["engineexternal"]	= os.realpath(dirs.root .. "Source/Engine/External/")
dirs["imgui"]			= os.realpath(dirs.root .. "Source/Engine/External/DearImGui/")
dirs["graphicsengine"]	= os.realpath(dirs.root .. "Source/Engine/GraphicsEngine/")
dirs["gameEngine"] 		= os.realpath(dirs.root .. "Source/Engine/GameEngine/")
dirs["assetmanager"] 	= os.realpath(dirs.root .. "Source/Engine/AssetManager/")
dirs["shaders"] 		= os.realpath(dirs.root .. "Source/Engine/Shaders/")
dirs["utilities"] 		= os.realpath(dirs.root .. "Source/Engine/Utilities/")
dirs["commonutilities"] = os.realpath(dirs.root .. "Source/Engine/Utilities/CommonUtilities/")
dirs["logger"] 			= os.realpath(dirs.root .. "Source/Engine/Utilities/Logger/")
dirs["content"] 		= os.realpath(dirs.root .. "Bin/Content/")
dirs["compiledshaders"] = os.realpath(dirs.root .. "Intermediate/Shaders/")


if not os.isdir (dirs.bin) then
	os.mkdir (dirs.bin)
	os.mkdir (dirs.content)
end


if not os.isdir(dirs.projectfiles) then
	os.mkdir (dirs.projectfiles)
end