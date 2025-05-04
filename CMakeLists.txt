cmake_minimum_required(VERSION 3.17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif (POLICY CMP0091)

# By default, the version information is extracted from the git index. However,
# we can override this behavior by explicitly setting ED_DOCK_MENU and
# skipping the git checks. This is useful for cases where this project is being
# used independently of its original git repo (e.g. vendored in another project)

if(NOT ED_DOCK_MENU_VERSION)
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
    include(GetGitRevisionDescription)
    git_describe(GitTagVersion --tags)
    string(FIND "${GitTagVersion}" "NOTFOUND" foundIndex)

    # Check if the substring was found
    if(foundIndex GREATER_EQUAL 0)
        set(VERSION_SHORT "1.0.0")
        message(WARNING "GitTagVersion is 'NOTFOUND', set version = ${VERSION_SHORT}")
    else()
        message(STATUS "GitTagVersion does not contain 'NOTFOUND'")
        string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_MAJOR "${GitTagVersion}")
        string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${GitTagVersion}")
        string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${GitTagVersion}")
        set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
    endif()
else()
    string(REGEX MATCHALL "[\.]" VERSION_DOT_MATCHES ${ED_DOCK_MENU_VERSION})
    list(LENGTH VERSION_DOT_MATCHES VERSION_DOT_COUNT)
    if(VERSION_DOT_COUNT EQUAL 2)
        set(VERSION_SHORT ${ED_DOCK_MENU_VERSION})
    else()
        message(WARNING "ED_DOCK_MENU_VERSION must be in major.minor.patch format, e.g. 1.2.3. Got ${ED_DOCK_MENU_VERSION}")
        set(VERSION_SHORT "1.0.0")
    endif()
endif()


project(QtEDDockMenu LANGUAGES CXX VERSION ${VERSION_SHORT})

option(ED_BUILD_STATIC     "Build the static library"  ON)
option(ED_BUILD_EXAMPLES   "Build the examples"        OFF)

add_subdirectory(src)

if(ED_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

