#OBJS specifies which files to compile as part of the project 
OBJS = main.cpp rendererSdl.cpp rendererCairo.cpp rendererGva.cpp pngget.cpp debug.cpp
#CC specifies which compiler we're using 
CC = g++ 
#COMPILER_FLAGS specifies the additional compilation options we're using 
# -w suppresses all warnings 
COMPILER_FLAGS = -w $(shell pkg-config --libs --cflags cairo)
#LINKER_FLAGS specifies the libraries we're linking against 
LINKER_FLAGS = -lSDL2 -lX11 -lpng -pthread 
#OBJ_NAME specifies the name of our exectuable 
OBJ_NAME = main 
#This is the target that compiles our executable 
all : $(OBJS) 
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

indent : $(OBJS)
	indent -gnu -nut $(OBJS)
	echo $(OBJS)

doxygen :
	doxygen ./Doxyfile
