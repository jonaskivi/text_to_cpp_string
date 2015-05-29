#!lua

-- A solution contains projects, and defines the available configurations
solution "text_to_cpp_string"
   configurations { "Debug", "Release" }
   platforms {"native", "x64", "x32"}

   -- A project defines one build target
   project "text_to_cpp_string"
      kind "ConsoleApp"
      language "C++"
      targetdir "./"
      files { "src/*.hpp", "src/*.cpp" }
      includedirs { "src" }
      
      configuration { "windows" }
         defines { "_CRT_SECURE_NO_DEPRECATE" } -- to use fopen on windows without warnings.
      
      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }
         debugdir "./"

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }
         debugdir "./"

