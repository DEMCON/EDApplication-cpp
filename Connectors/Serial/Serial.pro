#-------------------------------------------------
#
# Project created by QtCreator 2019-01-27T11:55:12
#
#-------------------------------------------------

QT              += serialport widgets

TARGET = Serial
TEMPLATE = lib
CONFIG += staticlib plugin

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Serial.cpp \
        ../DebugProtocolV0/ApplicationLayerV0.cpp \
        ../DebugProtocolV0/PresentationLayerV0.cpp \
        ../DebugProtocolV0/TransportLayerV0.cpp \
        ../../EmbeddedDebugger/Medium/Register/Register.cpp \
        ../../EmbeddedDebugger/Medium/Register/RegisterListModel.cpp \
        ../../EmbeddedDebugger/Medium/CPU/Cpu.cpp \
        ../../EmbeddedDebugger/Medium/CPU/CpuListModel.cpp \
        ../../EmbeddedDebugger/Medium/Medium.cpp \
        ../../Profiles/kconcatenaterowsproxymodel.cpp \
    Settingsdialog.cpp

HEADERS += \
        Serial.h \
        ../DebugProtocolV0/ApplicationLayerV0.h \
        ../DebugProtocolV0/DebugProtocolV0Enums.h \
        ../DebugProtocolV0/PresentationLayerV0.h \
        ../DebugProtocolV0/TransportLayerV0.h \
        ../BaseInterface/ApplicationLayerBase.h \
        ../BaseInterface/PresentationLayerBase.h \
        ../BaseInterface/TransportLayerBase.h \
        ../../EmbeddedDebugger/Medium/Register/Register.h \
        ../../EmbeddedDebugger/Medium/Register/RegisterListModel.h \
        ../../EmbeddedDebugger/Medium/CPU/Cpu.h \
        ../../EmbeddedDebugger/Medium/CPU/CpuListModel.h \
        ../../EmbeddedDebugger/Medium/Medium.h \
        ../BaseInterface/Common.h \
        ../../Profiles/kconcatenaterowsproxymodel.h \
    Settingsdialog.h

TARGET          = $$qtLibraryTarget(Serial)
DESTDIR         = ../../plugins
INCLUDEPATH += ../../EmbeddedDebugger/

FORMS += \
    Settingsdialog.ui

