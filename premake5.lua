term.pushColor(term.yellow)
print("Workspace: jpp")
term.popColor()
require("premake", ">=5.0.0-beta4")

local _arch = _OPTIONS["arch"]
if not _arch then
  _arch = os.hostarch()
  -- HACK: On my machine os.hostarch() returns x86 and not x86_64,
  --       probably related to Windows.
  if _arch == "x86" and os.is64bit() then
    _arch = "x86_64"
  end
end

newoption {
  trigger = "jpp-test-static-link",
  description = "Statically link jpp when building jpp-test",
  category = "Build Options"
}

workspace "jpp"
  architecture (_arch)
  configurations { "Debug", "Release" }
  startproject "jpp-test"

include "jpp"

project "jpp-test"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++17"

  location  "build/jpp-test"
  targetdir "build/jpp-test/%{cfg.system}_%{cfg.architecture}/%{cfg.buildcfg}"
  objdir    "build/jpp-test/%{cfg.system}_%{cfg.architecture}/%{cfg.buildcfg}/obj"

  includedirs "include"
  files "src/jpp-test/**.cpp"

  filter "options:jpp-test-static-link"
    defines "JPP_TEST_STATIC_LINK"
    links "jpp"

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
    symbols "On"

  filter "configurations:Release*"
    optimize "Speed"
