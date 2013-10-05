CXX = clang++
CC = clang
AR = ar

DEFINES   += -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DBOOST_ALL_NO_LIB -DLOAD_X11 -DNDEBUG -DRELEASE
INCLUDES  += -I./glimg -I../include
CPPFLAGS  += -MMD -MP $(DEFINES) $(INCLUDES)
CFLAGS    += $(CPPFLAGS) $(ARCH) -O3 -fomit-frame-pointer -Wall
CXXFLAGS  += $(CFLAGS)

SOURCES  = DdsLoader.cpp
SOURCES += ImageFormat.cpp
SOURCES += ImageSet.cpp
SOURCES += StbLoader.cpp
SOURCES += Util.cpp
SOURCES += ImageSetImpl.cpp
SOURCES += ImageCreator.cpp
SOURCES += stb_image.c

OBJECTS := $(SOURCES:.cpp=.o)
OBJECTS := $(OBJECTS:.c=.o)
OBJECTS := $(addprefix ../temp/, $(OBJECTS))

HEADERS  = ../include/glimg/ImageSet.h
HEADERS += ../include/glimg/StbLoader.h
HEADERS += ../include/glimg/glimg.h
HEADERS += ../include/glimg/ImageCreator.h
HEADERS += ../include/glimg/DdsLoader.h
HEADERS += ../include/glimg/ImageFormat.h
HEADERS += ../include/glimg/Loaders.h
HEADERS += ../include/glimg/ImageCreatorExceptions.h
HEADERS += ./glimg/ImageSetImpl.h
HEADERS += ./glimg/stb_image.h
HEADERS += ./glimg/OldDdsFmtConv.inc
HEADERS += ./glimg/DdsLoaderInt.h
HEADERS += ./glimg/Util.h

../lib/libglimg.a: ../temp $(OBJECTS)
	$(AR) -rcs ../lib/libglimg.a $(OBJECTS)

../temp/stb_image.o: ./glimg/stb_image.c ./glimg/stb_image.h
	$(CC) $(CFLAGS) -o "$@" -c "$<"

../temp/%.o: ./glimg/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

../temp:
	mkdir ../temp -p

OBJECTS_D = $(OBJECTS:.o=.d)

clean:
	rm -f $(OBJECTS) $(OBJECTS_D) ../lib/libglm.a -f

.PHONY: clean
