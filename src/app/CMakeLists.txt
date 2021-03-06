if (DEBUG)
	add_subdirectory(test)
endif ()

if (BLP AND MPQ AND MDLX AND MAP)
	include_directories("${CMAKE_CURRENT_BINARY_DIR}/../map")

	add_executable(wc3converter wc3converter.cpp)
	target_link_libraries(wc3converter ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})

	if (USE_QBLP)
		find_package(Qt5Core REQUIRED)
		target_link_libraries(wc3converter Qt5::Core qblp)
		include_directories(${CMAKE_SOURCE_DIR}/src/editor/qblp)
	endif ()

	install(TARGETS wc3converter DESTINATION bin)
endif ()

if (JASS)
	add_executable(jassc jassc.cpp)
	target_link_libraries(jassc ${wc3lib_CORE_LIBRARIES} wc3libjass ${Boost_LIBRARIES})
	install(TARGETS jassc DESTINATION bin)
endif ()

if (MAP AND JASS)
	add_executable(jasstrans jasstrans.cpp)
	target_link_libraries(jasstrans ${wc3lib_CORE_LIBRARIES} wc3libjass ${Boost_LIBRARIES})
	install(TARGETS jasstrans DESTINATION bin)
endif ()

if (MAP)
	add_executable(tilesetter tilesetter.cpp)
	target_link_libraries(tilesetter ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})
	install(TARGETS tilesetter DESTINATION bin)
endif ()

if (MPQ)
	add_executable(mpq mpq.cpp)
	target_link_libraries(mpq ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})
	install(TARGETS mpq DESTINATION bin)
endif ()

if (MAP)
	add_executable(wc3trans wc3trans.cpp)
	target_link_libraries(wc3trans ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})
	install(TARGETS wc3trans DESTINATION bin)
endif ()

if (MAP)
	add_executable(wc3triggervalidate wc3triggervalidate.cpp)
	target_link_libraries(wc3triggervalidate ${wc3lib_CORE_LIBRARIES} ${Boost_LIBRARIES})
	install(TARGETS wc3triggervalidate DESTINATION bin)
endif ()
