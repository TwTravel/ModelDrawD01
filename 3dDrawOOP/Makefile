CXX = g++ -g -pg  -fprofile-arcs -ftest-coverage -fpermissive
CFLAGS = -fPIC -fpermissive
MYFLAGS = -DTIME_INFO -Wno-deprecated  -fpermissive
INC = -I ./include -I  ./TyGL 

CXXFLAGS = $(INC)
LDFLAGS =

SOURCES = \
    ./TyGL/api.cpp \
    ./3DS.cpp \
    ./TyGL/zbuffer.cpp \
    ./TyGL/vertex.cpp \
    ./TyGL/ztriangle.cpp \
    ./TyGL/kh_project.cpp \
    ./TyGL/light.cpp \
    ./TyGL/matrix.cpp \
    ./TyGL/clip.cpp \
    ./TyGL/specbuf.cpp \
    ./TyGL/image_util.cpp \
    ./TyGL/init.cpp \
    ./TyGL/glx.cpp \
    ./main.cpp 

OBJ = \
    ./TyGL/api.o \
    ./3DS.o \
    ./TyGL/zbuffer.o \
    ./TyGL/vertex.o \
    ./TyGL/ztriangle.o \
    ./TyGL/kh_project.o \
    ./TyGL/light.o \
    ./TyGL/matrix.o \
    ./TyGL/clip.o \
    ./TyGL/specbuf.o \
    ./TyGL/image_util.o \
    ./TyGL/init.o \
    ./TyGL/glx.o \
    ./main.o 

Decoder: $(OBJ) main.o
	$(CXX) $(CXXFLAGS) -o Draw3D $(OBJ) $(LDFLAGS)  -ldl -lpthread

clean:
	-rm Draw3D *.gc*  TyGL/*.gc* $(OBJ)
