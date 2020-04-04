PROGS = ext2_cp ext2_ln ext2_ls ext2_mkdir ext2_rm ext2_rm_bonus readimage test
all : $(PROGS)

readimage : readimage.o utilities.o
	gcc -Wall -g -o readimage $^

ext2_cp : ext2_cp.o utilities.o
	gcc -Wall -g -o ext2_cp $^

ext2_ln : ext2_ln.o utilities.o 
	gcc -Wall -g -o ext2_ln $^

ext2_ls : ext2_ls.o utilities.o 
	gcc -Wall -g -o ext2_ls $^

ext2_mkdir : ext2_mkdir.o utilities.o 
	gcc -Wall -g -o ext2_mkdir $^

ext2_rm : ext2_rm.o utilities.o 
	gcc -Wall -g -o ext2_rm $^

ext2_rm_bonus : ext2_rm_bonus.o utilities.o 
	gcc -Wall -g -o ext2_rm_bonus $^

test : test.o utilities.o 
	gcc -Wall -g -o test $^

%.o : %.c ext2.h utilities.h
	gcc -Wall -g -c $^

clean : 
	rm -f $(PROGS) *.o