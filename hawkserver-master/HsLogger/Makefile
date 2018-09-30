#---------------------------------------------
#添加规则
include ../proj.linux/Rules.mk

#修改中间文件生成路径
OBJ_DIR = .

#---------------------------------------------
#~~~~~~统一路径设置
INC_DIRS += -I../../HawkSDK/include
LIB_DIRS += -L../../HawkSDK/lib
LNK_LIBS += -lhawklog -lhawkutil

#---------------------------------------------
#~~~~~~生成目标设置
GEN_EXE  =  ../proj.linux/$(BIN_DIR)/hslogger

#---------------------------------------------
#~~~~~~编译标签设置
all: $(OBJ_FILES)
	$(CPP) $(CPPFLAGS) $(OBJ_FILES) $(LIB_DIRS) $(LNK_LIBS) -o $(GEN_EXE)
	
#-------------------------------------------------------------------------
#~~~~~~生成依赖文件
depend: $(SRC_FILES)
	$(RM)  .depend
	$(CPP) -MM $(MACROS) $(INC_DIRS) $^ > .depend

clean: force
	$(RM) *.o
	$(RM) $(OUT_DIR)

force:

#-------------------------------------------------------------------------
#~~~~~~添加依赖
ifeq (.depend,$(wildcard .depend))
include .depend
endif

