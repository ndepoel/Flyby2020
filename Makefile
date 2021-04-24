# On any Debian-based Linux system, install the following packages to build:
# libsdl2-dev libsdl2-image-dev libvorbis-dev libopenal-dev
OUTDIR = out
BIN = Flyby
OBJ = \
    Game.o \
    Renderer/Renderer.o \
    Math3D/Interpolate.o \
    Math3D/Math3D.o \
    TextureManager/TextureManager.o \
    GLCommon.o \
    Camera/AxisCamera.o \
    Camera/Camera.o \
    Entities/DemoCamera.o \
    Entities/Dragon.o \
    Entities/CameraTarget.o \
    Entities/Speaker.o \
    Entities/TargetPosition.o \
    Entities/Timeshift.o \
    Entities/Visor.o \
    Entities/ParticleEmitter.o \
    Entities/Magdalena.o \
    Entity/EntityMap.o \
    Entity/EntityParser.o \
    Frustum/Frustum.o \
    Main.o \
    SoundManager/WavInput.o \
    SoundManager/OggInput.o \
    SoundManager/SoundManager.o \
    Font/Font.o \
    Shared/StringParser.o \
    World/BezierPatch.o \
    World/BSPDraw.o \
    World/BSPTree.o \
    World/BSPLoad.o \
    World/Skybox.o \
    World/BezierFace.o \
    ModelManager/ModelManager.o \
    SharedVars.o \
    Model/RawModel.o \
    Model/AnimSequence.o \
    Model/MD3Model.o \
    Model/MD2Model.o
PCH = 

CC = g++ -DNO_GLEXT
INCLUDE = 
LIB = -lGL -lGLU -lSDL2 -lSDL2main -lSDL2_image -lopenal -logg -lvorbis -lvorbisfile

all : $(PCH) $(OBJ)
	$(CC) -o $(OUTDIR)/$(BIN) $(OBJ) $(LIB)

rebuild: clean all

clean:
	rm -rf $(PCH) $(OBJ)
	rm -rf $(OUTDIR)/*

%.o : %.cpp
	$(CC) -c $< $(INCLUDE) -o $@
	
%.gch : %
	$(CC) -x c++ -c $< $(INCLUDE) -o $@
