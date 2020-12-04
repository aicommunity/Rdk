set(VERSION)
find_package(Hg)
if(HG_FOUND)
    HG_WC_INFO(${PROJECT_SOURCE_DIR} Rdk)
    message("Current Rdk revision is ${Rdk_WC_REVISION}")
    message("Current Rdk changeset is ${Rdk_WC_CHANGESET}")
    set(VERSION "${Rdk_WC_REVISION}")
endif()
set(DEF_VERSION "-DRDK_CORE_VERSION=${VERSION}")

ADD_DEFINITIONS(-DLIBRDK_LIBRARY_EXPORT
        #-DRDK_UNICODE_RUN
        -DRDK_QT
        -DBOOST_PYTHON_STATIC_LIB
        -DQT_NO_VERSION_TAGGING
        -DBOOST_NUMPY_STATIC_LIB
        ${DEF_VERSION}
        )

if("RDK_USE_CUDA" IN_LIST FOUND_DEFINES)
    message("RDK_USE_CUDA defined")
    ADD_DEFINITIONS(-DGPU)
endif()

if("RDK_USE_PYTHON" IN_LIST FOUND_DEFINES)
    message("RDK_USE_PYTHON defined")
    set(RDK_PYTHON_MAJOR 3)
    set(RDK_PYTHON_MINOR 6)
endif()