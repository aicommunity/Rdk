
DEFINES += LIBRDK_LIBRARY_EXPORT
DEFINES += RDK_UNICODE_RUN
DEFINES += RDK_QT
DEFINES += BOOST_PYTHON_STATIC_LIB
DEFINES += BOOST_NUMPY_STATIC_LIB
DEFINES += QT_NO_VERSION_TAGGING

exists($$PWD/../../../../Build/Qt/BuildAll/RdkDefinesApp.pri) {
    include($$PWD/../../../../Build/Qt/BuildAll/RdkDefinesApp.pri)
}


contains(DEFINES,RDK_USE_CUDA) {
        DEFINES += GPU
}

contains(DEFINES,RDK_USE_PYTHON) {
    RDK_PYTHON_MAJOR = 3
    RDK_PYTHON_MINOR = 6
}

contains(DEFINES,RDK_USE_OPENCV) {

    unix {
        contains(DEFINES, OPENCV_4) {
            INCLUDEPATH += $$(OPENCV4_PATH)/include/opencv4
        }else{
            INCLUDEPATH += $$(OPENCV3_PATH)/include
        }
    }

    windows {
        contains(DEFINES, OPENCV_4) {
            INCLUDEPATH += $$(OPENCV4_PATH)/build/include/opencv4
        }else{
            INCLUDEPATH += $$(OPENCV3_PATH)/build/include
        }
    }
}

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
}

windows {
    INCLUDEPATH += $$(BOOST_PATH)

    contains(DEFINES,RDK_USE_PYTHON) {
        INCLUDEPATH += $$(ANACONDA_PATH)/include/
        INCLUDEPATH += $$(ANACONDA_PATH)/Lib/site-packages/numpy/core/include/numpy
    }

    contains(DEFINES, GPU) {
         INCLUDEPATH += $$(CUDA_PATH)/include
    }
}

contains(DEFINES, RDK_USE_TENSORFLOW) {


# TF
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

} else:unix {

}

}

