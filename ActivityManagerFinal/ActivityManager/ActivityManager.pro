QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    addeventdialog.cpp \
    addreminderdialog.cpp \
    addtododialog.cpp \
    main.cpp \
    mainwindow.cpp \
    uivisitor.cpp

HEADERS += \
    addeventdialog.h \
    addreminderdialog.h \
    addtododialog.h \
    mainwindow.h \
    model/Utility/JsonSaveVisitor.h \
    uivisitor.h

#QMAKE_CXXFLAGS += -Wno-implicit-function-declaration
#QMAKE_CXXFLAGS += -Wnonportable-include-path

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += \
    $$PWD/model \
    $$PWD/model/items \
    $$PWD/model/Utility

SOURCES += \
    model/Activity.cpp \
    model/ActivityManager.cpp \
    model/items/ToDo.cpp \
    model/items/Event.cpp \
    model/items/Reminder.cpp

HEADERS += \
    model/Activity.h \
    model/ActivityManager.h \
    model/items/ToDo.h \
    model/items/Event.h \
    model/items/Reminder.h \
    model/Utility/ItemVisitor.h \
    model/Utility/ConstItemVisitor.h

RESOURCES += resources.qrc
ICON = calendar.icns

DISTFILES += \
    Dockerfile \
    model/icons/delete-button.png \
    model/icons/load-button.png \
    model/icons/save-button.png


