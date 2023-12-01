#Transformations example

file(
 GLOB_RECURSE terrainTesting_INC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.h *.hpp
)

file(
 GLOB_RECURSE terrainTesting_SRC CONFIGURE_DEPENDS
 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
 *.c *.cpp
)
#Copies this terrainTesting_proceduralGeometry's asset folder to bin when it is built
add_custom_target(copyAssetsATerrain ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
${CMAKE_CURRENT_SOURCE_DIR}/assets/
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/)

install(FILES ${terrainTesting_INC} DESTINATION include/terrainTesting_proceduralGeometry)
add_executable(terrainTesting_proceduralGeometry ${terrainTesting_SRC} ${terrainTesting_INC} ${terrainTesting_ASSETS})
target_link_libraries(terrainTesting_proceduralGeometry PUBLIC core IMGUI)
target_include_directories(terrainTesting_proceduralGeometry PUBLIC ${CORE_INC_DIR} ${stb_INCLUDE_DIR})

#Trigger asset copy when terrainTesting_proceduralGeometry is built
add_dependencies(terrainTesting_proceduralGeometry copyAssetsATerrain)