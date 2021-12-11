win32-msvc* {
    MSVC_VER = $$(VisualStudioVersion)
    equals(MSVC_VER, 14.0){
        MSVC_COMPILER = vc14
    }
    equals(MSVC_VER, 15.0){
        MSVC_COMPILER = vc15
    }
    equals(MSVC_VER, 16.0){
        MSVC_COMPILER = vc16
    }
    equals(MSVC_VER, 17.0){
        MSVC_COMPILER = vc17
    }
#    message("VC Compiler: "$${MSVC_COMPILER})
}

unix {
QMAKE_CXXFLAGS += -Wno-misleading-indentation -Wno-deprecated-copy
}

windows:!windows-g++ { # visual studio spec filter
    QMAKE_CXXFLAGS_EXCEPTIONS_ON = /EHa
    QMAKE_CXXFLAGS_STL_ON = /EHa
}

DEFINES += LIBRDK_LIBRARY_EXPORT
DEFINES += RDK_UNICODE_RUN
DEFINES += RDK_QT
DEFINES += BOOST_PYTHON_STATIC_LIB
DEFINES += BOOST_NUMPY_STATIC_LIB
DEFINES += BOOST_BIND_GLOBAL_PLACEHOLDERS
DEFINES += QT_NO_VERSION_TAGGING

windows {
# fix build bug with boost+python under visual studio
DEFINES += HAVE_SNPRINTF
}


exists($$PWD/../../../../Build/Qt/BuildAll/RdkDefinesApp.pri) {
    include($$PWD/../../../../Build/Qt/BuildAll/RdkDefinesApp.pri)
}


contains(DEFINES,RDK_USE_CUDA) {
        DEFINES += GPU
}

contains(DEFINES,RDK_USE_PYTHON) {
    RDK_PYTHON_MAJOR = 3
    RDK_PYTHON_MINOR = 8
}

contains(DEFINES,RDK_USE_OPENCV) {

# функция добавляет постфикс(второй параметр) ко всем элементам первого входного параметра
defineReplace(addPostfix) {
 libList = $$1
 for(lib, libList) {
  returnValue += $${lib}$${2}
 }
 return($$returnValue)
}

    unix {
        contains(DEFINES, OPENCV_4) {
            INCLUDEPATH += $$(OPENCV4_PATH)/include/opencv4
            OPENCV_LIB_PATH = $$(OPENCV4_PATH)
        }else{
            INCLUDEPATH += $$(OPENCV3_PATH)/include
            OPENCV_LIB_PATH = $$(OPENCV3_PATH)
        }

        OPENCV_UNIX_LINKER_LINE = -L$${OPENCV_LIB_PATH}/lib $$OPENCV_LIBS_LIST
    }

    windows {
        contains(DEFINES, OPENCV_4) {
            OPENCV_LIBS_VERSION = 454
            OPENCV_PATH=$$(OPENCV4_PATH)
        }else{
            OPENCV_LIBS_VERSION = 345
            OPENCV_PATH=$$(OPENCV3_PATH)
        }

        INCLUDEPATH += $${OPENCV_PATH}/build/include

        contains(DEFINES, RDK_USE_CUDA) {
            OPENCV_COMPILED_VERSION_64 = $${MSVC_COMPILER}cuda
            OPENCV_COMPILED_VERSION_86 = $${MSVC_COMPILER}
        } else {
            OPENCV_COMPILED_VERSION_64 = $${MSVC_COMPILER}
            OPENCV_COMPILED_VERSION_86 = $${MSVC_COMPILER}
        }

        !contains(QMAKE_TARGET.arch, x86_64) {
            OPENCV_LIB_PATH = $${OPENCV_PATH}/build/x86/$${OPENCV_COMPILED_VERSION_86}/lib
        } else {
            OPENCV_LIB_PATH = $${OPENCV_PATH}/build/x64/$${OPENCV_COMPILED_VERSION_64}/lib
        }

        CONFIG(debug, debug|release) {
#            message("VideoAnalytics: using OpenCv from "$${OPENCV_LIB_PATH}/Debug)
            OPENCV_WIN_LINKER_LINE = -L$${OPENCV_LIB_PATH}/Debug $$addPostfix($$OPENCV_LIBS_LIST, $${OPENCV_LIBS_VERSION}d)
        }

        CONFIG(release, debug|release) {
#            message("VideoAnalytics: using OpenCv from "$${OPENCV_LIB_PATH}/Release)
            OPENCV_WIN_LINKER_LINE = -L$${OPENCV_LIB_PATH}/Release $$addPostfix($$OPENCV_LIBS_LIST, $${OPENCV_LIBS_VERSION})
        }
    }
}


#Boost
unix {
    INCLUDEPATH += $$(BOOST_PATH)
    INCLUDEPATH += $$(BOOST_PATH)/include

    contains(DEFINES, GPU) {
        INCLUDEPATH += /usr/local/cuda/include
    }


    contains(DEFINES,RDK_USE_PYTHON) {
        ANACONDA_PATH=$$(ANACONDA_PATH)
        isEmpty(ANACONDA_PATH) {
            INCLUDEPATH += /usr/include/numpy
            INCLUDEPATH += /usr/include/python$${RDK_PYTHON_MAJOR}.$${RDK_PYTHON_MINOR}
            INCLUDEPATH += /usr/include/python$${RDK_PYTHON_MAJOR}.$${RDK_PYTHON_MINOR}/numpy
        } else{
            INCLUDEPATH += $$(ANACONDA_PATH)/include/python$${RDK_PYTHON_MAJOR}.$${RDK_PYTHON_MINOR}m/
            INCLUDEPATH += $$(ANACONDA_PATH)/lib/python$${RDK_PYTHON_MAJOR}.$${RDK_PYTHON_MINOR}/site-packages/numpy/core/include/numpy/
        }
    }

    BOOST_UNIX_LINKER_LINE += -L$$(BOOST_PATH)/lib -lboost_system \
        -lboost_system \
        -lboost_chrono \
        -lboost_thread \
        -lboost_program_options \
        -lboost_filesystem \
        -lboost_date_time \
        -lboost_timer

    # Следующие две строки должна быть строго выше чем строки линковки питона из анаконды!!!
    BOOST_UNIX_LINKER_LINE += -L$$(QTDIR)/lib -lQt5Core -lQt5Widgets -lQt5Gui -lQt5PrintSupport -lGL
    BOOST_UNIX_LINKER_LINE += -L/usr/lib/x86_64-linux-gnu -lcurl

    contains(DEFINES, RDK_USE_PYTHON) {
        BOOST_UNIX_LINKER_LINE += -L$$(BOOST_PATH)/lib -lboost_python$${RDK_PYTHON_MAJOR}$${RDK_PYTHON_MINOR} \
            -lboost_numpy$${RDK_PYTHON_MAJOR}$${RDK_PYTHON_MINOR}

    isEmpty(ANACONDA_PATH) {
         BOOST_UNIX_LINKER_LINE += -L/usr/lib/python$${RDK_PYTHON_MAJOR}.$${RDK_PYTHON_MINOR}/config-$${RDK_PYTHON_MAJOR}.$${RDK_PYTHON_MINOR}m-x86_64-linux-gnu -lpython$${RDK_PYTHON_MAJOR}.$${RDK_PYTHON_MINOR}m -lpython$${RDK_PYTHON_MAJOR}.$${RDK_PYTHON_MINOR}
         BOOST_UNIX_LINKER_LINE += -L/usr/lib/python$${RDK_PYTHON_MAJOR}.$${RDK_PYTHON_MINOR}/config-$${RDK_PYTHON_MAJOR}.$${RDK_PYTHON_MINOR}m-aarch64-linux-gnu -lpython$${RDK_PYTHON_MAJOR}.$${RDK_PYTHON_MINOR}m -lpython$${RDK_PYTHON_MAJOR}.$${RDK_PYTHON_MINOR}
    } else {
         BOOST_UNIX_LINKER_LINE += -L$$(ANACONDA_PATH)/lib -lpython$${RDK_PYTHON_MAJOR}.$${RDK_PYTHON_MINOR}m -lpython$${RDK_PYTHON_MAJOR} #-lpng -lssl
    }
}

}

windows {
    BOOST_COMPILED_VERSION = msvc-$$(VisualStudioVersion)
    INCLUDEPATH += $$(BOOST_PATH)

    contains(DEFINES,RDK_USE_PYTHON) {
        INCLUDEPATH += $$(ANACONDA_PATH)/include/
        INCLUDEPATH += $$(ANACONDA_PATH)/Lib/site-packages/numpy/core/include/numpy
    }

    contains(DEFINES, GPU) {
         INCLUDEPATH += $$(CUDA_PATH)/include
    }

    !contains(QMAKE_TARGET.arch, x86_64) {
        BOOST_WIN_LINKER_LINE += -L$$(BOOST_PATH)/$${BOOST_COMPILED_VERSION}-x86/lib/
    } else {
         BOOST_WIN_LINKER_LINE += -L$$(BOOST_PATH)/$${BOOST_COMPILED_VERSION}-x64/lib/
    }

    BOOST_WIN_LINKER_LINE +=   -lWldap32
    BOOST_WIN_LINKER_LINE +=   -lAdvapi32

    BOOST_WIN_LINKER_LINE += -L$$(ANACONDA_PATH)/libs/
}

contains(DEFINES, RDK_USE_DARKNET) {
    unix {
        DARKNET_UNIX_LINKER_LINE+= -L$$PWD/../../../Libraries/Rdk-DarknetLib/ThirdParty/darknet -ldarknet
    }
}

contains(DEFINES, RDK_USE_TENSORFLOW) {
    INCLUDEPATH += $$(TENSORFLOW_PATH)/bazel-genfiles
    INCLUDEPATH += $$(TENSORFLOW_PATH)/bazel-tensorflow/external/eigen_archive
    INCLUDEPATH += $$(TENSORFLOW_PATH)/bazel-tensorflow/external/protobuf_archive/src
    INCLUDEPATH += $$(TENSORFLOW_PATH)/bazel-tensorflow/external/com_google_protobuf/src
    INCLUDEPATH += $$(TENSORFLOW_PATH)/bazel-tensorflow/external/com_google_absl
    INCLUDEPATH += $$(TENSORFLOW_PATH)/bazel-bin/tensorflow
    DEPENDPATH += $$(TENSORFLOW_PATH)/bazel-bin/tensorflow
    INCLUDEPATH += $$(TENSORFLOW_PATH)
    DEPENDPATH += $$(TENSORFLOW_PATH)/bazel-bin/tensorflow

    windows {
        TENSORFLOW_WIN_LINKER_LINE += -L$$(TENSORFLOW_PATH)/bazel-bin/tensorflow -ltensorflow_framework.dll.if -ltensorflow.dll.if -ltensorflow_cc.dll.if
    } else:unix {
        TENSORFLOW_UNIX_LINKER_LINE += -L$$(TENSORFLOW_PATH)/bazel-bin/tensorflow -ltensorflow_cc -ltensorflow_framework
    }
}

contains(DEFINES,RDK_USE_MATLAB) {
    INCLUDEPATH += $$(MATLAB_PATH)/extern/include/
    windows {
        MATLAB_WIN_LINKER_LINE = -llibeng -L$$(MATLAB_PATH)/extern/lib/win64/microsoft/ \
        -llibmx -L$$(MATLAB_PATH)/extern/lib/win64/microsoft/ \
        -llibMatlabEngine -L$$(MATLAB_PATH)/extern/lib/win64/microsoft/ \
        -llibMatlabCppSharedLib -L$$(MATLAB_PATH)/extern/lib/win64/microsoft/ \
        -llibMatlabDataArray -L$$(MATLAB_PATH)/extern/lib/win64/microsoft/
    } else:unix {


    }

}

