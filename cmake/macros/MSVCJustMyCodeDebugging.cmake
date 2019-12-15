# - Enable Just My Code debugging on MSVC
#
#  include(MSVCJustMyCodeDebugging)
#
# Requires these CMake modules:
#  ListCombinations.cmake

if(MSVC AND NOT "${MSVC_VERSION}" LESS 1918)
	# Only available in VS 2017 15.8 and newer
	include(ListCombinations)
	list_combinations(_varnames
		PREFIXES
		CMAKE_CXX_FLAGS_
		SUFFIXES
		DEBUG
        RELWITHDEBINFO)
	foreach(_var ${_varnames})
		set(${_var} "${${_var}} /JMC")
	endforeach()
endif()
