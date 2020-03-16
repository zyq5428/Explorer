CROSS_COMPILE 	?= 
TARGET		  	?= bbb
CFLAGS			?= -g -lpthread

CC 				:= $(CROSS_COMPILE)gcc
LD				:= $(CROSS_COMPILE)ld
OBJCOPY 		:= $(CROSS_COMPILE)objcopy
OBJDUMP 		:= $(CROSS_COMPILE)objdump

INCDIRS 		:= common \
				   oled \
				   sht3x \
				   			   
SRCDIRS			:= project \
				   common \
				   oled \
				   sht3x \

INCLUDE			:= $(patsubst %, -I %, $(INCDIRS))

SFILES			:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.S))
CFILES			:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))

SFILENDIR		:= $(notdir  $(SFILES))
CFILENDIR		:= $(notdir  $(CFILES))

SOBJS			:= $(patsubst %, obj/%, $(SFILENDIR:.S=.o))
COBJS			:= $(patsubst %, obj/%, $(CFILENDIR:.c=.o))
OBJS			:= $(SOBJS) $(COBJS)

VPATH			:= $(SRCDIRS)
	
#$(TARGET).bin : $(OBJS)
#	$(LD) -Timx6ul.lds -o $(TARGET).elf $^
#	$(OBJCOPY) -O binary -S $(TARGET).elf $@
#	$(OBJDUMP) -D -m arm $(TARGET).elf > $(TARGET).dis

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(SOBJS) : obj/%.o : %.S
	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $<

$(COBJS) : obj/%.o : %.c
	$(CC) -c $(CFLAGS) $(INCLUDE) -o $@ $<

.PHONY: clean
clean:
	rm -rf $(TARGET) $(COBJS) $(SOBJS)
