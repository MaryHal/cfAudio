solution "cfAudio"
    configurations { "Debug", "Release" }
    platforms { "native", "x32", "x64" }

    ---------------------------------------
    -- Static Library
    project "build_lib"
        language "C++"
        buildoptions { "-std=c++11" }
        -- kind "StaticLib"
        kind "SharedLib"

        includedirs { "include" }
        files { "src/**" }

        targetdir "lib"
        targetname "cfAudio"

        ---------------------------------------
        -- Link static libraries and config
        libdirs ("lib")
        links { "openal", "sndfile" }

        configuration "linux"
            -- links { "GL", "X11", "Xrandr", "pthread", "m", "glfw3" }

        configuration "windows"
            -- links { "glfw3", "opengl32" }

        configuration { "native or x64" }
            targetsuffix "64"

        configuration "x32"
            targetsuffix "32"

        ---------------------------------------
        -- Build rules
        configuration "Debug"
            defines "DEBUG"
            flags { "Symbols", "ExtraWarnings" }

        configuration "Release"
            flags { "Symbols", "Optimize", "ExtraWarnings" }



    project "build_test"
        language "C++"
        buildoptions { "-std=c++11" }

        includedirs { "include/" }
        files { "test/**" }

        targetdir "bin"
        targetname "Test"

        ---------------------------------------
        -- Link static libraries and config
        libdirs ("lib")
        links { "openal", "sndfile" }

        configuration "linux"
            targetprefix "linux_"

        configuration "windows"
            targetprefix "windows_"

        configuration { "native or x64" }
            links { "cfAudio64" }
            targetsuffix "64"

        configuration "x32"
            links { "cfAudio32" }
            targetsuffix "32"

        ---------------------------------------
        -- Build rules
        configuration "Debug"
            kind "ConsoleApp"
            defines "DEBUG"
            flags { "Symbols", "ExtraWarnings" }

        configuration "Release"
            kind "ConsoleApp"
            flags { "Symbols", "Optimize", "ExtraWarnings" }
