DIR_OBJ = ./obj
DIR_BIN = ./bin

OBJ_C = $(wildcard ${DIR_OBJ}/*.cpp)

OBJ_O = $(patsubst %.cpp,${DIR_BIN}/%.o,$(notdir ${OBJ_C}))

TARGET = main
#BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = g++

DEBUG = -g -O0 -Wall
CFLAGS += $(DEBUG)

LIB = -lwiringPi -lm -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -pthread


${TARGET}:${OBJ_O} ${OBJ_Opp}
	$(CC) $(CFLAGS) $(OBJ_O) -o $@ -I /usr/include/opencv4 $(LIB)

${DIR_BIN}/%.o : $(DIR_OBJ)/%.cpp
	$(CC) $(CFLAGS) -c  $< -o $@ -I /usr/include/opencv4 $(LIB)

clean :
	rm $(DIR_BIN)/*.* 
	rm $(TARGET) 