workspace "asphyxia"
	configurations { "Debug", "Release" }
	system "windows"
	architecture "x64"

	project "cstrike"
		location "cstrike"
		targetname "cstrike"
		kind "SharedLib"
        defines { "NOMINMAX"}
		language "C++"
		cppdialect "c++20"

		-- specify physical include file paths
		files
		{
			"cstrike/**.cpp",
			"cstrike/**.h",

            -- core [features, sdk, etc...]
            "cstrike/core/**.cpp",
			"cstrike/core/**.h",

            -- features
            "cstrike/features/**.h",
            "cstrike/features/**.cpp",
                "cstrike/features/misc/**.h",
                "cstrike/features/misc/**.cpp",
                "cstrike/features/visuals/**.h",
                "cstrike/features/visuals/**.cpp",

            -- sdk
            "cstrike/sdk/**.h",
            "cstrike/sdk/**.cpp",
                "cstrike/sdk/datatypes/**.h",
                "cstrike/sdk/datatypes/**.cpp",
                "cstrike/sdk/interfaces/**.h",
                "cstrike/sdk/interfaces/**.cpp",

            -- utilities
            "cstrike/utilities/**.h",
            "cstrike/utilities/**.cpp",


			-- general like: stb_X library, xor_str, etc...
			"dependencies/*.h",
			"dependencies/*.hpp",

			-- imgui
			"dependencies/imgui/**.cpp",
			"dependencies/imgui/**.h",

			-- minhook
			"dependencies/minhook/**.h",
			"dependencies/minhook/**.c",
			"dependencies/minhook/hde/**.h",
			"dependencies/minhook/hde/**.c",

            -- extension
            "extension/**.h",

            -- resources [fonts, images, etc...]
			"resources/*.h"
		}

		-- specify virtual filter file paths
		vpaths
		{
			{ ["*"] = "cstrike/*" },

			-- core [features, sdk, etc...]
			{ ["core/*"] = "cstrike/core/*" },
			-- utilities
			{ ["utilities/*"] = "cstrike/utilities/*" },

			-- dependencies
			{ ["dependencies/*"] = "dependencies/*" },
			{ ["dependencies/imgui/*"] = "dependencies/imgui/*" },
			{ ["dependencies/safetyhook/*"] = "dependencies/safetyhook/*" },

			-- extension
			{ ["extension/*"] = "extension/*" },

			-- resources [fonts, images, etc...]
			{ ["resources/*"] = "resources/*.h" }
		}

		-- @note: use the "!" prefix to force a specific directory using msvs's provided environment variables instead of premake tokens
		targetdir "$(SolutionDir)build/$(Configuration)/"
		objdir "!$(SolutionDir)intermediate/$(ProjectName)/$(Configuration)/"
		implibname "$(OutDir)$(TargetName)"

		-- link
		includedirs {
			"$(SolutionDir)dependencies", 
			"$(SolutionDir)dependencies/freetype/include",
		}

		libdirs { "$(SolutionDir)dependencies/freetype/binary" }
		links { "d3d11.lib" }
		flags { "MultiProcessorCompile", "NoImportLib", "NoManifest", "NoPCH" } -- @test: NoImplicitLink

		-- compile
		conformancemode "on"
		editandcontinue "off"
		entrypoint "CoreEntryPoint"
		exceptionhandling "off"
		staticruntime "on"
		symbols "full"

		-- configuration specific
		filter "configurations:Debug"
			defines { "_DEBUG" }
			flags { "NoIncrementalLink" }
			justmycode "off"
			rtti "on"
            links { "freetype_debug.lib" }
			--sanitize { "Address" } --@test

		filter "configurations:Release"
			defines { "NDEBUG" }
			flags { "LinkTimeOptimization" } -- @test: NoRuntimeChecks
			optimize "speed"
			rtti "off"
            links { "freetype.lib" }
			--buildoptions { "/Zc:threadSafeInit-" }-- @test: "/Zc:threadSafeInit-" to disable thread-safe local static initialization ('__Init_thread_header'/'__Init_thread_footer' calls)
