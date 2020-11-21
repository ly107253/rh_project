
CC=gcc
CCC=g++
TR=strip
NM=nm
AR=ar
LD=ld

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

