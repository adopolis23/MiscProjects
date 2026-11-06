workspace "SpeedRead"
    architecture "x86_64"
    configurations { "Debug", "Release" }

project "FileGenerator"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++23"

    targetdir "./"
    objdir "./"

    files {
        "./FileGenerator.cpp",
    }

    includedirs {
    }

project "FileReader"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++23"

    targetdir "./"
    objdir "./"

    files {
        "./FileReader.cpp",
    }

    includedirs {
    }
