project "shaderc"
    kind "StaticLib"
    language "C++"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "./include/shaderc/*"
    }

    includedirs
    {
        "include"
    }
    
    filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "off"
        
    filter { "system:windows", "configurations:Release" }
        buildoptions "/MT"
