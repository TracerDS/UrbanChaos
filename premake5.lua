ROOT_PATH = path.getabsolute('.')
PROJ_PATH = path.join(ROOT_PATH, 'Projects')
EXTERN_PATH = path.join(ROOT_PATH, 'extern')
local OBJ_PATH = path.join(PROJ_PATH, 'obj')
local LIB_PATH = path.join(EXTERN_PATH, 'lib')
local INCLUDE_PATH = path.join(EXTERN_PATH, 'include')

print(('-'):rep(100))
print('Configuration info:')
print('ROOT_PATH: ',    ROOT_PATH,    "\t\t | path.getabsolute('.')")
print('PROJ_PATH: ',    PROJ_PATH,    " \t | path.join(ROOT_PATH, 'extern')")
print('EXTERN_PATH: ',  EXTERN_PATH,  " \t | path.join(ROOT_PATH, 'extern')")
print('OBJ_PATH: ',     OBJ_PATH,     " | path.join(PROJ_PATH, 'obj')")
print('LIB_PATH: ',     LIB_PATH,     " | path.join(EXTERN_PATH, 'lib')")
print('INCLUDE_PATH: ', INCLUDE_PATH, " | path.join(EXTERN_PATH, 'include')")
print(('-'):rep(100),'\n')

workspace 'Urban Chaos'
    configurations { 'Debug', 'Release' }
    platforms { 'x86', 'x64' }
    location ( PROJ_PATH )
    kind 'WindowedApp'
    language 'C++'
    objdir ( OBJ_PATH )

    cppdialect 'C++17'
    flags { 'MultiProcessorCompile' }

    defines { 'NOMINMAX', 'WIN32_LEAN_AND_MEAN' }

    defaultplatform 'x86'
    links { 'SDL2.lib', 'OpenAL32.lib' }
    libdirs { LIB_PATH }
    includedirs { INCLUDE_PATH }

    filter { 'platforms:x86' }
        architecture 'x86'

    filter { 'platforms:x64' }
        architecture 'x64'
    
    filter 'configurations:Debug'
        defines { 'DEBUG' }
        targetsuffix '_d'
        optimize 'Debug'
        symbols 'Full'

    filter 'configurations:Release'
        defines { 'NDEBUG' }
        optimize 'Speed'
    
	filter "system:windows"
        toolset "v143"
		preferredtoolarchitecture "x86_64"
    
    filter {}
        include 'Fallen'