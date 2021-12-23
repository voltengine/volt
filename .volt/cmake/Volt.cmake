include_guard(GLOBAL)

# Set output directory to ${CMAKE_BINARY_DIR}/bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/bin/")
endforeach()

# Read and iterate ./cache/packages.txt
file(STRINGS "${CMAKE_SOURCE_DIR}/cache/packages.txt" VOLT_PATHS)
foreach(VOLT_PATH_ITEM ${VOLT_PATHS})
	# Extract name and path from paths array item
	string(FIND ${VOLT_PATH_ITEM} " " VOLT_SPACE_INDEX)
	string(SUBSTRING ${VOLT_PATH_ITEM} 0 ${VOLT_SPACE_INDEX} VOLT_PACKAGE_NAME)
	math(EXPR VOLT_SPACE_INDEX "${VOLT_SPACE_INDEX} + 1")
	string(SUBSTRING ${VOLT_PATH_ITEM} ${VOLT_SPACE_INDEX} -1 VOLT_PACKAGE_PATH)

	# Add package's CMakeLists.txt if it's present
	if(EXISTS "${VOLT_PACKAGE_PATH}/CMakeLists.txt")
		message("[Volt CLI] Adding package: ${VOLT_PACKAGE_PATH}")
		add_subdirectory(${VOLT_PACKAGE_PATH} "${CMAKE_BINARY_DIR}/dependencies/${VOLT_PACKAGE_NAME}")
	endif()
endforeach()

# Clean up temporary variables
unset(VOLT_PATHS)
unset(VOLT_PATH_ITEM)
unset(VOLT_SPACE_INDEX)
unset(VOLT_PACKAGE_NAME)
unset(VOLT_PACKAGE_PATH)
