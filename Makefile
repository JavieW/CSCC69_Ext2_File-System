PROGS = ext2_cp ext2_ln ext2_ls ext2_mkdir ext2_rm ext2_rm_bonus
all : $(PROGS)

ext2_cp : ext2_cp.o
	gcc -Wall -g -o ext2_cp $^

ext2_ln : ext2_ln.o
	gcc -Wall -g -o ext2_ln $^

ext2_ls : ext2_ls.o
	gcc -Wall -g -o ext2_ls $^

ext2_mkdir : ext2_mkdir.o
	gcc -Wall -g -o ext2_mkdir $^

ext2_rm : ext2_rm.o
	gcc -Wall -g -o ext2_rm $^

ext2_rm_bonus : ext2_rm_bonus.o
	gcc -Wall -g -o ext2_rm_bonus $^

%.o : %.c ext2.h
	gcc -Wall -g -c $<

clean : 
	rm -f $(PROGS) *.o