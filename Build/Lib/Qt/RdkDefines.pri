#DEFINES += RDK_USE_CUDA
#DEFINES += RDK_USE_PYTHON

unix {
#    DEFINES += RDK_USE_DARKNET
}

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


unix {
    INCLUDEPATH += $$(BOOST_PATH)/include
    INCLUDEPATH += $$(OPENCV3_PATH)/include

    contains(DEFINES, GPU) {
        INCLUDEPATH += /usr/local/cuda-9.0/include
}


    contains(DEFINES,RDK_USE_PYTHON) {
        ANACONDA_PATH=$$(ANACONDA_PATH)
        isEmpty(ANACONDA_PATH) {
            INCLUDEPATH += /usr/include/python3.5
            INCLUDEPATH += $$(HOME)/.local/lib/python3.5/site-packages/numpy/core/include/numpy/
            INCLUDEPATH += /home/user/.local/lib/python3.5/site-packages/numpy/core/include/numpy
        } else{
            INCLUDEPATH += $$(ANACONDA_PATH)/include/python3.5m/
            INCLUDEPATH += $$(ANACONDA_PATH)/lib/python3.5/site-packages/numpy/core/include/numpy/
        }
    }
}

windows {
    INCLUDEPATH += $$(BOOST_PATH)

    contains(DEFINES,RDK_USE_PYTHON) {
        INCLUDEPATH += $$(ANACONDA_PATH)/include/
        INCLUDEPATH += $$(ANACONDA_PATH)/Lib/site-packages/numpy/core/include/numpy
}
    INCLUDEPATH += $$(BOOST_PATH)
    INCLUDEPATH += $$(OPENCV3_PATH)/build/include

    contains(DEFINES, GPU) {
        INCLUDEPATH += $$(CUDA_PATH)/include
}
}

