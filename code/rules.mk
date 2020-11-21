
CC=arm-linux-gnueabihf-gcc
CCC=arm-linux-gnueabihf-g++
TR=arm-linux-gnueabihf-strip
NM=arm-linux-gnueabihf-nm
AR=arm-linux-gnueabihf-ar
LD=arm-linux-gnueabihf-ld

MAKE = make



CFLAGS = -Wall -Werror -O0 -g -s -rdynamic -funwind-tables -ffunction-sections -I$(WORK_DIR) 
DFLAGS = -D DEVICE_TYPE_JZQ1

DEPS := $(patsubst %.o, %.c.dep, $(OBJS))
TEMPDEPS := $(addsuffix ., $(dir $(DEPS)))
DEPS := $(join $(TEMPDEPS), $(notdir $(DEPS)))

IGNORE=$(wildcard .*.c.dep)
-include $(IGNORE)


	
%.o: %.c
	@echo "  CC  " $< "-o" $@; \
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@ -Wp,-MMD,$(dir $<).$(notdir $<).dep

