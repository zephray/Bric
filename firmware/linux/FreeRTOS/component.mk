INCLUDES += \
	-I $(COMPONENT_PATH)/Source/include

COMPONENT_INCLUDES += \
	-I $(COMPONENT_PATH)/Linux/portable/GCC/Linux \
	-I $(COMPONENT_PATH)/Source/include

CSRCS +=\
	$(COMPONENT_PATH)/Source/croutine.c \
	$(COMPONENT_PATH)/Source/event_groups.c \
	$(COMPONENT_PATH)/Source/list.c \
	$(COMPONENT_PATH)/Source/queue.c \
	$(COMPONENT_PATH)/Source/stream_buffer.c \
	$(COMPONENT_PATH)/Source/tasks.c \
	$(COMPONENT_PATH)/Source/timers.c \
	$(COMPONENT_PATH)/Source/portable/MemMang/heap_4.c \
	$(COMPONENT_PATH)/Linux/portable/GCC/Linux/port.c
