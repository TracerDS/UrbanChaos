project 'Fallen'
    kind 'ConsoleApp'

    vpaths {
        ['Headers/*'] = { 'Headers/**.h', 'Headers/**.hpp' },
        ['Sources/*'] = { 'Source/**.c', 'Source/**.cpp' },
        ['*'] = 'premake5.lua'
    }

    files {
        'Headers/**.h',
        'Headers/**.hpp',
        'Sources/**.c',
        'Sources/**.cpp',
    }
    
	filter "architecture:not x86"
        flags { "ExcludeFromBuild" }
