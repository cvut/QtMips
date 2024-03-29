project(gui
        LANGUAGES C CXX
        VERSION ${MAIN_PROJECT_VERSION}
        DESCRIPTION "Graphical UI for the simulator")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(gui_SOURCES
        aboutdialog.cpp
        cachedock.cpp
        cacheview.cpp
        cop0dock.cpp
        coreview/adder.cpp
        coreview/alu.cpp
        coreview/and.cpp
        coreview/connection.cpp
        coreview/constant.cpp
        coreview/instructionview.cpp
        coreview/junction.cpp
        coreview/latch.cpp
        coreview/logicblock.cpp
        coreview/memory.cpp
        coreview/minimux.cpp
        coreview/multiplexer.cpp
        coreview/multitext.cpp
        coreview/programcounter.cpp
        coreview/registers.cpp
        coreview/value.cpp
        coreview.cpp
        extprocess.cpp
        fontsize.cpp
        gotosymboldialog.cpp
        graphicsview.cpp
        memorydock.cpp
        memorymodel.cpp
        memorytableview.cpp
        messagesdock.cpp
        messagesmodel.cpp
        messagesview.cpp
        newdialog.cpp
        hexlineedit.cpp
        highlighterasm.cpp
        highlighterc.cpp
        hinttabledelegate.cpp
        lcddisplaydock.cpp
        lcddisplayview.cpp
        main.cpp
        mainwindow.cpp
        peripheralsdock.cpp
        peripheralsview.cpp
        programdock.cpp
        programmodel.cpp
        programtableview.cpp
        registersdock.cpp
        savechangeddialog.cpp
        srceditor.cpp
        statictable.cpp
        terminaldock.cpp
        textsignalaction.cpp
        )
set(gui_HEADERS
        aboutdialog.h
        cachedock.h
        cacheview.h
        cop0dock.h
        coreview/adder.h
        coreview/alu.h
        coreview/and.h
        coreview/connection.h
        coreview/constant.h
        coreview/coreview_colors.h
        coreview/instructionview.h
        coreview/junction.h
        coreview/latch.h
        coreview/logicblock.h
        coreview/memory.h
        coreview/minimux.h
        coreview/multiplexer.h
        coreview/multitext.h
        coreview/programcounter.h
        coreview/registers.h
        coreview/value.h
        coreview.h
        extprocess.h
        fontsize.h
        gotosymboldialog.h
        graphicsview.h
        memorydock.h
        memorymodel.h
        memorytableview.h
        messagesdock.h
        messagesmodel.h
        messagesview.h
        newdialog.h
        hexlineedit.h
        highlighterasm.h
        highlighterc.h
        hinttabledelegate.h
        lcddisplaydock.h
        lcddisplayview.h
        mainwindow.h
        peripheralsdock.h
        peripheralsview.h
        programdock.h
        programmodel.h
        programtableview.h
        registersdock.h
        savechangeddialog.h
        srceditor.h
        statictable.h
        terminaldock.h
        textsignalaction.h
        )
set(gui_UI
    gotosymboldialog.ui
    NewDialog.ui
    peripheralsview.ui
    MainWindow.ui
    NewDialogCache.ui
    )
set(gui_RESOURCES
        icons.qrc
        samples.qrc
        )


if("${WASM}")
	message(STATUS "gui :: Including WASM only files.")
	list(APPEND gui_SOURCES qhtml5file_html5.cpp)
	list(APPEND gui_HEADERS qhtml5file.h)
endif()

# MACOS
set(ICON_NAME gui)
set(ICON_PATH ${CMAKE_SOURCE_DIR}/data/icons/macos/${ICON_NAME}.icns)
# END MACOS

add_executable(gui
               ${ICON_PATH}
               ${gui_SOURCES}
               ${gui_HEADERS}
               ${gui_UI}
               ${gui_RESOURCES})
target_include_directories(gui PUBLIC . coreview)
target_link_libraries(gui
        PRIVATE ${QtLib}::Core ${QtLib}::Widgets ${QtLib}::Gui
        PRIVATE machine os_emulation assembler)
target_compile_definitions(gui
                           PRIVATE
                           APP_ORGANIZATION=\"${MAIN_PROJECT_ORGANIZATION}\"
                           APP_ORGANIZATION_DOMAIN=\"${MAIN_PROJECT_HOMEPAGE_URL}\"
                           APP_GIT=\"${MAIN_PROJECT_HOMEPAGE_URL}\"
                           APP_NAME=\"${MAIN_PROJECT_NAME}\"
                           APP_VERSION=\"${MAIN_PROJECT_VERSION}\"
                           ENV_CONFIG_FILE_NAME=\"${MAIN_PROJECT_NAME_UPPER}_CONFIG_FILE\")
set_target_properties(gui PROPERTIES
                      OUTPUT_NAME "${MAIN_PROJECT_NAME_LOWER}_${PROJECT_NAME}")

if(${${QtLib}PrintSupport_FOUND})
    target_link_libraries(gui PRIVATE ${QtLib}::PrintSupport)
    target_compile_definitions(gui PRIVATE WITH_PRINTING=1)
endif()

# MACOS
set_property(SOURCE ${ICON_PATH}
             PROPERTY MACOSX_PACKAGE_LOCATION Resources)
set_target_properties(gui PROPERTIES
                      MACOSX_BUNDLE true
                      MACOSX_BUNDLE_GUI_IDENTIFIER cz.cvut.fel.${MAIN_PROJECT_ORGANIZATION}.gui
                      MACOSX_BUNDLE_BUNDLE_NAME ${MAIN_PROJECT_NAME}
                      MACOSX_BUNDLE_BUNDLE_VERSION "${MAIN_PROJECT_VERSION}"
                      MACOSX_BUNDLE_SHORT_VERSION_STRING "${MAIN_PROJECT_VERSION}"
                      MACOSX_BUNDLE_ICONFILE ${ICON_NAME}
                      )
# END MACOS

# =============================================================================
# Installation
# =============================================================================

# Prior to CMake version 3.13, installation must be performed in the subdirectory,
# there the target was created. Therefore executable installation is to be found
# in corresponding CMakeLists.txt.

install(TARGETS gui
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION ${EXECUTABLE_OUTPUT_PATH}
        )

