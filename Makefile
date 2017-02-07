CXX      = x86_64-w64-mingw32-g++
CXXFLAGS = -std=c++03 -Os -ffast-math -g3 -Wno-write-strings
LDFLAGS  = -mwindows -static-libgcc -static-libstdc++
LDLIBS   = -lopengl32 -lwinmm -lglu32 -lgdi32

objects = building.o camera.o car.o deco.o entity.o glbbox.o            \
 glmatrix.o glquat.o glrgba.o glvector2.o glvector3.o ini.o light.o     \
 math.o mesh.o random.o render.o sky.o texture.o visible.o win.o        \
 world.o

all : pixelcity.exe

clean :
	rm -f pixelcity.exe $(objects)

pixelcity.exe : $(objects)
	$(CXX) $(LDFLAGS) -o pixelcity.exe $(objects) $(LDLIBS)

building.o: building.cpp glTypes.h building.h entity.h deco.h light.h \
 mesh.h macro.h math.h random.h texture.h world.h win.h
camera.o: camera.cpp glTypes.h ini.h macro.h math.h world.h win.h
car.o: car.cpp glTypes.h building.h entity.h car.h camera.h mesh.h \
 macro.h math.h random.h render.h texture.h world.h visible.h win.h
deco.o: deco.cpp glTypes.h deco.h entity.h light.h mesh.h macro.h math.h \
 random.h render.h texture.h world.h visible.h
entity.o: entity.cpp camera.h glTypes.h entity.h macro.h math.h render.h \
 texture.h world.h visible.h win.h
glbbox.o: glbbox.cpp macro.h glTypes.h
glmatrix.o: glmatrix.cpp macro.h glTypes.h
glquat.o: glquat.cpp math.h glTypes.h
glrgba.o: glrgba.cpp math.h glTypes.h macro.h
glvector2.o: glvector2.cpp glTypes.h math.h macro.h
glvector3.o: glvector3.cpp macro.h math.h glTypes.h
ini.o: ini.cpp glTypes.h ini.h win.h
light.o: light.cpp glTypes.h camera.h entity.h light.h macro.h math.h \
 random.h render.h texture.h visible.h
math.o: math.cpp macro.h math.h
mesh.o: mesh.cpp glTypes.h mesh.h
random.o: random.cpp random.h
render.o: render.cpp glTypes.h entity.h car.h camera.h ini.h light.h \
 macro.h math.h render.h sky.h texture.h world.h win.h
sky.o: sky.cpp camera.h glTypes.h macro.h math.h random.h render.h sky.h \
 texture.h world.h
texture.o: texture.cpp glTypes.h building.h entity.h camera.h car.h \
 light.h macro.h random.h render.h sky.h texture.h world.h win.h
visible.o: visible.cpp glTypes.h camera.h macro.h math.h visible.h \
 world.h win.h
win.o: win.cpp camera.h glTypes.h car.h entity.h ini.h macro.h random.h \
 render.h texture.h win.h world.h visible.h
world.o: world.cpp glTypes.h building.h entity.h car.h deco.h camera.h \
 light.h macro.h math.h mesh.h random.h render.h sky.h texture.h \
 visible.h win.h world.h
