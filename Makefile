PROGS = ext2_cp ext2_ln ext2_ls ext2_mkdir ext2_rm ext2_rm_bonus readimage
all : $(PROGS)

readimage : readimage.o untilities.o
	gcc -Wall -g -o readimage $^

ext2_cp : ext2_cp.o untilities.o
	gcc -Wall -g -o ext2_cp $^

ext2_ln : ext2_ln.o untilities.o 
	gcc -Wall -g -o ext2_ln $^

ext2_ls : ext2_ls.o untilities.o
	gcc -Wall -g -o ext2_ls $^

ext2_mkdir : ext2_mkdir.o untilities.o
	gcc -Wall -g -o ext2_mkdir $^

ext2_rm : ext2_rm.o untilities.o
	gcc -Wall -g -o ext2_rm $^

ext2_rm_bonus : ext2_rm_bonus.o untilities.o
	gcc -Wall -g -o ext2_rm_bonus $^

%.o : %.c ext2.h untilities.h
	gcc -Wall -g -c $^

clean : 
	rm -f $(PROGS) *.o