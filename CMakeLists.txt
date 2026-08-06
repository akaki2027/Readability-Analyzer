
cmake_minimum_required(VERSION 3.16)
project("ReadabilityAnalyzer")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Test)
find_package(Threads REQUIRED)

set(COMMON_SOURCES
    "Document.cpp" "Document.hpp"
    "Token.cpp" "Token.hpp"
    "Syllabizer.cpp" "Syllabizer.hpp"
    "Lexicon.cpp" "Lexicon.hpp"
    "ThreadSafeQueue.hpp"
)

set(GUI_SOURCES
    "guimain.cpp"
    "MainWindow.cpp" "MainWindow.hpp"
    "DocumentWindow.cpp" "DocumentWindow.hpp"
    "DocumentWorker.cpp" "DocumentWorker.hpp"
)

add_executable(main "main.cpp" ${COMMON_SOURCES})
add_executable(guimain ${GUI_SOURCES} ${COMMON_SOURCES})
add_executable(testproject "ProjectTest.cpp" "ProjectTest.h" "MainWindow.cpp" "MainWindow.hpp" "DocumentWindow.cpp" "DocumentWindow.hpp" "DocumentWorker.cpp" "DocumentWorker.hpp" ${COMMON_SOURCES}
)

target_link_libraries(main PRIVATE Qt6::Core Threads::Threads)
target_link_libraries(guimain PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui Threads::Threads)
target_link_libraries(testproject PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui Qt6::Test Threads::Threads)
target_compile_definitions(testproject PRIVATE QT_QPA_PLATFORM=offscreen)

find_package(Doxygen OPTIONAL_COMPONENTS dot)
if(DOXYGEN_FOUND)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile")
        add_custom_target(docs ALL
            COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            COMMENT "Generating Doxygen documentation..."
            VERBATIM
        )
    else()
        message(STATUS "Doxyfile not found")
    endif()
else()
    message(STATUS "Doxygen not found")
endif()