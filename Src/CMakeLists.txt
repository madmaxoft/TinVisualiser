# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set (CMAKE_INCLUDE_CURRENT_DIR ON)





set(SOURCES
	MainWindow.cpp
	TinData.cpp
	TinDataLoader.cpp
	TinDataSaver.cpp
	TinVisualiser.cpp
	ZoomingGraphicsView.cpp
)

set(HEADERS
	MainWindow.hpp
	TinData.hpp
	TinDataLoader.hpp
	TinDataSaver.hpp
	ZoomingGraphicsView.hpp
)

set(UI
	MainWindow.ui
)

add_executable(TinVisualiser
	${SOURCES}
	${HEADERS}
	${UI}
)

target_link_libraries(TinVisualiser Qt6::Widgets)





# Process translations (update, compile):
qt_add_translations(TinVisualiser
	SOURCE_TARGETS TinVisualiser
	TS_FILE_DIR Translations
	RESOURCE_PREFIX Translations
)
