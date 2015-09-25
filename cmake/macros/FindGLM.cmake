################################################################################
# Custom cmake module for CEGUI to find the glm header
################################################################################
include(FindPackageHandleStandardArgs)

find_path(GLM_H_PATH NAMES glm/glm.hpp glm.hpp)
mark_as_advanced(GLM_H_PATH)

find_package_handle_standard_args(GLM DEFAULT_MSG GLM_H_PATH)


# set up output vars
if (GLM_FOUND)
    set (GLM_INCLUDE_DIR ${GLM_H_PATH})
else()
    set (GLM_INCLUDE_DIR)
endif()

