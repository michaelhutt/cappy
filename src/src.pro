
TEMPLATE = app
FORMS = ../res/main.ui
SOURCES = cappy.cc cappywindow.cpp ui/CircleDecomposer.cpp
HEADERS += cappywindow.h ui/CircleDecomposer.hpp ui/CircleItem.hh

#LIBS += -lhighgui -lcv -lcxcore

TARGET = cappy

DESTDIR = ../build/
OBJECTS_DIR = $$DESTDIR/.obj
MOC_DIR = $$DESTDIR/.moc
RCC_DIR = $$DESTDIR/.qrc
UI_DIR = $$DESTDIR/.ui
