INCLUDEPATH += $$system(pwd)/network

HEADERS += $$system(pwd)/network/multiserverclientabstraction.h \
		$$system(pwd)/network/networkmagic.h \
		$$system(pwd)/network/bytearraymessagesizepeekerforiodevice.h \
		$$system(pwd)/network/abstractserverconnection.h \
                $$system(pwd)/network/iprotocolknower.h \
                $$system(pwd)/network/iprotocolknowerfactory.h \
                $$system(pwd)/network/abstractconnection.h

SOURCES += $$system(pwd)/network/multiserverclientabstraction.cpp \
		$$system(pwd)/network/networkmagic.cpp \
		$$system(pwd)/network/bytearraymessagesizepeekerforiodevice.cpp \
                $$system(pwd)/network/abstractserverconnection.cpp \
                $$system(pwd)/network/abstractconnection.cpp

include($$system(pwd)/ssltcpclient.pri)
