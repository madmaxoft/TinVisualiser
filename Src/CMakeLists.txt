set(SOURCES
	MainWindow.cpp
	TinData.cpp
	TinDataLoader.cpp
	TinDataSaver.cpp
	TinVisualiser.cpp
)

set(HEADERS
	MainWindow.hpp
	TinData.hpp
	TinDataLoader.hpp
	TinDataSaver.hpp
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
