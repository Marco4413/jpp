require("premake", ">=5.0.0-beta4")

project "jpp"
  kind "StaticLib"
  language "C++"
  cppdialect "C++17"

  location  "build/jpp"
  targetdir "build/jpp/%{cfg.system}_%{cfg.architecture}/%{cfg.buildcfg}"
  objdir    "build/jpp/%{cfg.system}_%{cfg.architecture}/%{cfg.buildcfg}/obj"

  includedirs "include"
  files { "include/jpp/src/**.cpp", "include/jpp/**.h" }

  filter "toolset:clang"
    buildoptions {
      "-Wall", "-Wextra", "-Wpedantic", "-Werror",
      "-Wno-gnu-zero-variadic-macro-arguments"
    }

  filter "toolset:gcc"
    buildoptions { "-Wall", "-Wextra", "-Wpedantic", "-Werror" }

  filter "action:vs*"
    fatalwarnings { "All" }
    warnings "Extra"
    externalwarnings "Extra"

  filter "toolset:msc"
    buildoptions { "/W4", "/WX" }

  filter "configurations:Debug*"
    defines "JPP_DEBUG"
    symbols "On"

  filter "configurations:Release*"
    optimize "Speed"
