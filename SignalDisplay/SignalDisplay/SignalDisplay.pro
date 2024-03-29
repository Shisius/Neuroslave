QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dialogconnectionsettings.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    sarstructsettings/sarsessionitem.cpp \
    sarstructsettings/sarstructsettingsModel.cpp \
    sarstructsettings/sarstructsettingsdialog.cpp \
    stringlist_dialog.cpp \
    ../../libc/neuroslave/definitions/neuroslave_struct.cpp

HEADERS += \
    dialogconnectionsettings.h \
    mainwindow.h \
    qcustomplot.h \
    sarstructsettings/sarsessionitem.h \
    sarstructsettings/sarstructsettingsModel.h \
    sarstructsettings/sarstructsettingsdialog.h \
    stringlist_dialog.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    dialogconnectionsettings.ui

INCLUDEPATH += ../ \
               ../../libc/neuroslave/definitions/ \
               ../../libc/third_party/rapidjson

RESOURCES += \
    img.qrc

