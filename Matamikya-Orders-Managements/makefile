CC = gcc
OBJS1 = matamikya.o matamikya_orders_management.o 
OBJS2 = amount_set_str.o amount_set_str_main.o amount_set_str_tests.o
EXEC1 = matamikya
EXEC2 = amount_set_str
DEBUG_FLAG = # now empty, assign -g for debug
COMP_FLAG = -std=c99 -Wall -Werror
$(EXEC1) : $(OBJS1)
	$(CC) $(DEBUG_FLAG) $(OBJS1) -std=c99 matamikya_print.c \
	tests/matamikya_main.c tests/matamikya_tests.c -L. -lmtm -lm -las -o $@
matamikya.o:matamikya.c matamikya.h set.h amount_set.h \
 matamikya_orders_management.h matamikya_print.h list.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
matamikya_orders_management.o:matamikya_orders_management.c matamikya_orders_management.h amount_set.h \
 set.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c



$(EXEC2) : $(OBJS2)
	$(CC) $(DEBUG_FLAG) $(OBJS2) -std=c99 -L. -lmtm -lm -las -o $@	
amount_set_str.o:amount_set_str.c amount_set_str.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
amount_set_str_main.o:amount_set_str_main.c amount_set_str_tests.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
amount_set_str_tests.o:amount_set_str_tests.c amount_set_str.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c

clean:
	rm -f $(OBJS1) $(EXEC1) $(OBJS2) $(EXEC2)



