add_definitions ( "-wd4996" "-D_CRT_SECURE_NO_WARNINGS" "-D_ITERATOR_DEBUG_LEVEL=0" )
add_definitions ( "-D_UNICODE" "-DUNICODE" "/utf-8" )

include (MSVCMultipleProcessCompile)
include (MSVCStaticRuntime)
