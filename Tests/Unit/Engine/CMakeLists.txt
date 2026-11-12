cmake_minimum_required(VERSION 3.16)

option(RDK_BUILD_LEGACY_ENGINE_UNIT_TESTS
    "Build legacy RDK engine unit tests (many targets need API refresh against current headers)"
    OFF)

# Contract/regression tests maintained against current RDK headers
add_subdirectory(UProperty)
# ActiveComponents order + PropertiesOrder contract tests (always on)
add_subdirectory(UComponent)
add_subdirectory(UContainer)

if(RDK_BUILD_LEGACY_ENGINE_UNIT_TESTS)
    add_subdirectory(UStorage)
    add_subdirectory(Logging)
endif()
