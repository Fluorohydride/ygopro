add_definitions ( "-wd4996" "-D_CRT_SECURE_NO_WARNINGS" )
add_definitions ( "-D_UNICODE" "-DUNICODE" "/utf-8" )

include (MSVCMultipleProcessCompile)
include (MSVCStaticRuntime)
include (MSVCJustMyCodeDebugging)
