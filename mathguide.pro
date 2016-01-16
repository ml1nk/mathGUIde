TEMPLATE = app
DESTDIR  = bin
TARGET = mathguide
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui

DEPENDPATH += ./ui
INCLUDEPATH += .
INCLUDEPATH += ./source
INCLUDEPATH += ./source/graph
QT += webkitwidgets

win32 { 
    # Pfade müssen vermutlich angepasst werden
    INCLUDEPATH += ./Python31/include
    LIBS += ./Python31/libs/python31.lib
    RC_FILE = mathguide.rc
}

macx {
    # Pfade müssen vermutlich angepasst werden
    INCLUDEPATH += /Library/Frameworks/Python.framework/Versions/3.1/Headers
    LIBS += /Library/Frameworks/Python.framework/Versions/3.1/lib/libpython3.1.dylib
    ICON = mathguide.icns
}

linux-g++ {
    INCLUDEPATH += /usr/include/python3.4m
    LIBS += -lpython3.4m
}

# Input
HEADERS += \
    source/mainwindow.h \
    source/constants.h \
    source/py-inter.h \
    source/py-highlighter.h \
    source/inputwindow.h \
    source/recordwindow.h \
    source/plotwindow.h \
    source/popupwidgets.h \
    source/dialogs.h \
    source/graph/graphwidget.h \
    source/graph/graphscene.h \
    source/graph/node.h \
    source/graph/edge.h
SOURCES += \
    source/main.cpp \
    source/constants.cpp \
    source/mainwindow.cpp \
    source/py-inter.cpp \
    source/py-highlighter.cpp \
    source/inputwindow.cpp \
    source/recordwindow.cpp \
    source/plotwindow.cpp \
    source/popupwidgets.cpp \
    source/dialogs.cpp \
    source/graph/graphwidget.cpp \
    source/graph/graphscene.cpp \
    source/graph/node.cpp \
    source/graph/edge.cpp
FORMS += \
    ui/optionsdlg.ui \
    ui/sumdlg.ui \
    ui/proddlg.ui \
    ui/listdlg.ui \
    ui/matrixdlg.ui \
    ui/plotdlg.ui \
    ui/tabledlg.ui \
    ui/valuetabledlgx.ui
TRANSLATIONS = mathguide_de.ts
RESOURCES += mathguide.qrc
