CC = g++
CFLAGS = -O2 -std=c++11
LFLAGS = -lGL -lGLEW -lglfw
OUTEXEC = iesire
OBJS = Camera.o Model3D.o Mesh.o Shader.o SkyBox.o stb_image.o OpenGL_4_Application_VS2015.o

all: build iesire

build:
	mkdir build/

iesire: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(OUTEXEC)

Camera.o: Camera.cpp Camera.hpp
	$(CC) $(CFLAGS) -c Camera.cpp

Model3D.o: Model3D.cpp Model3D.hpp Mesh.hpp stb_image.h tiny_obj_loader.h
	$(CC) $(CFLAGS) -c Model3D.cpp

Mesh.o: Mesh.cpp Mesh.hpp Shader.hpp
	$(CC) $(CFLAGS) -c Mesh.cpp

Shader.o: Shader.cpp Shader.hpp
	$(CC) $(CFLAGS) -c Shader.cpp

SkyBox.o: Shader.hpp stb_image.h SkyBox.cpp SkyBox.hpp
	$(CC) $(CFLAGS) -c SkyBox.cpp

stb_image.o: stb_image.c stb_image.h
	$(CC) $(CFLAGS) -c stb_image.c

OpenGL_4_Application_VS2015.o: OpenGL_4_Application_VS2015.cpp
	$(CC) $(CFLAGS) -c OpenGL_4_Application_VS2015.cpp

clean:
	rm $(OUTEXEC)
	rm $(OBJS)

clean_obj:
	rm $(OBJS)
