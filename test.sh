# #./readimage twolevel.img

# # ls test case: (dir, file, link) * (n/a, -a) + (invalid path, not exist)
# # check: correctness of output
# echo "-----test root: <<<./ext2_ls twolevel.img  />>>-----"
# ./ext2_ls twolevel.img  /

# echo "-----test root with flag: <<<./ext2_ls twolevel.img -a />>>-----"
# ./ext2_ls twolevel.img -a /

# echo "-----test root with flag: <<<./ext2_ls twolevel.img -a /lost+found>>>-----"
# ./ext2_ls twolevel.img -a /lost+found

# echo "-----test afile: <<<./ext2_ls twolevel.img  /afile>>>-----"
# ./ext2_ls twolevel.img /afile

# echo "-----test afile, <<<flag does't metter: ./ext2_ls twolevel.img -a /afile>>>-----"
# ./ext2_ls twolevel.img -a /afile

# echo "-----test afile, this should fail: <<<./ext2_ls twolevel.img -a /afile/>>>-----"
# ./ext2_ls twolevel.img -a /afile/

# echo "-----test level1: <<<./ext2_ls twolevel.img  /level1>>>-----"
# ./ext2_ls twolevel.img /level1

# echo "-----test level 1 end with '/': <<<./ext2_ls twolevel.img -a /level1/>>>-----"
# ./ext2_ls twolevel.img -a /level1/

# echo "-----test level2: <<<./ext2_ls twolevel.img -a /level1/level2>>>-----"
# ./ext2_ls twolevel.img -a /level1/level2

# echo "-----test bfile: <<<./ext2_ls twolevel.img -a /level1/level2/bfile>>>-----"
# ./ext2_ls twolevel.img -a /level1/level2/bfile

# echo "-----test bfile, this should fail: <<<./ext2_ls twolevel.img -a /level1/level2/bfile/>>>-----"
# ./ext2_ls twolevel.img -a /level1/level2/bfile/

# echo "-----test non-exist path, this should fail: <<<./ext2_ls twolevel.img -a /level1/level2/cfile>>>-----"
# ./ext2_ls twolevel.img -a /level1/level2/cfile

# echo "-----test no input image file, this should fail: <<<./ext2_ls twolevel.img>>>-----"
# ./ext2_ls twolevel.img

# # note: the bfilee.s file in the disk contain an 16 character! below testcase should actually fail
# echo "-----test print a file name, this should work but fail: <<<./ext2_ls onedirectory.img /level1/bfilee.s>>>-----"
# ./ext2_ls onedirectory.img /level1/bfilee.s

# # cp
# ./readimage emptydisk.img
# echo "\n---------------------\n"
# ./ext2_cp emptydisk.img /Users/javiewang/Desktop/large.txt /large.txt
# ./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename0.txt
# ./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename1.txt
# ./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename2.txt
# ./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename3.txt
# ./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename4.txt
# ./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename5.txt
# ./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename6.txt
# ./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename7.txt
# ./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename8.txt
# ./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename9.txt
# ./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename10.txt
# ./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename11.txt
# echo "\n---------------------\n"
# ./readimage emptydisk.img
# ./ext2_ls emptydisk.img -a /

# rm
# ./readimage emptydisk.img
# echo "\n---------------------\n"
# ./ext2_rm emptydisk.img /largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename7.txt
# echo "\n---------------------\n"
# ./readimage emptydisk.img
# ./ext2_ls emptydisk.img -a /


# mkdir + ls
# echo "\n----------mkdir--------\n"
# ./ext2_mkdir emptydisk.img /123
# echo "\n----------ls-----------\n"
# ./ext2_ls emptydisk.img /
# echo "\n---------readimage----\n"
# ./readimage emptydisk.img
# echo "\n---------test---------\n"
# ./test emptydisk.img /
# echo "\n---------test---------\n"
# ./test emptydisk.img /123
# echo "\n---------test---------\n"
# ./test emptydisk.img /lost+found

#mkdir
# echo "-----test mkdir at root, this should fail: <<<./ext2_mkdir emptydisk.img />>>-----"
# ./ext2_mkdir emptydisk.img /
# echo "-----test mkdir lost+found, this should fail: <<<./ext2_mkdir emptydisk.img /lost+found>>>-----"
# ./ext2_mkdir emptydisk.img /lost+found
# echo "-----test mkdir hello at root, this should work: <<<./ext2_mkdir emptydisk.img /hello>>>-----"
# ./ext2_mkdir emptydisk.img /hello
# ./ext2_ls emptydisk.img /
# echo "-----test mkdir hello at root again, this should fail: <<<./ext2_mkdir emptydisk.img /hello/>>>-----"
# ./ext2_mkdir emptydisk.img /hello/
# ./ext2_ls emptydisk.img /
# echo "-----test mkdir world at non-existing dir, this should fail: <<<./ext2_mkdir emptydisk.img /hi/world>>>-----"
# ./ext2_mkdir emptydisk.img /hi/world
# echo "-----test mkdir world at hello dir, this should work: <<<./ext2_mkdir emptydisk.img /hello/world>>>-----"
# ./ext2_mkdir emptydisk.img /hello/world
# ./ext2_ls emptydisk.img /hello
# echo "-----test mkdir good at hello dir, this should work: <<<./ext2_mkdir emptydisk.img /hello/good>>>-----"
# ./ext2_mkdir emptydisk.img /hello/good
# ./ext2_ls emptydisk.img /hello
# echo "-----test mkdir world at file afile, this should fail: <<<./ext2_mkdir twolevel.img /afile/world>>>-----"
# ./ext2_mkdir twolevel.img /afile/world
# echo "-----test mkdir . and .. at hello dir, this should fail: <<<./ext2_mkdir emptydisk.img /hello/.(..)>>>-----"
# ./ext2_mkdir emptydisk.img /hello/.
# ./ext2_mkdir emptydisk.img /hello/..
# echo "-----test mkdir with no input img file, this should fail: <<<./ext2_mkdir /hello/>>>-----"
# ./ext2_mkdir /hello/
# echo "-----test mkdir with no wrong img file, this should fail: <<<./ext2_mkdir haha.img /hello/>>>-----"
# ./ext2_mkdir haha.img /hello/

# rm (file, link, dir)*(target link count 1, 2, more)

# cp /Users/javiewang/Desktop/emptydisk.img /Users/javiewang/Desktop/CSCC69A3/emptydisk.img
# ./ext2_mkdir emptydisk.img /1
# ./ext2_mkdir emptydisk.img /1/2
# ./ext2_mkdir emptydisk.img /1/3
# ./ext2_mkdir emptydisk.img /1/3/4
# ./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /1/f1
# ./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /1/2/f2
# ./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /1/3/f3
# ./ext2_ln emptydisk.img -s /1/f1 /1/2/sl1
# ./ext2_ln emptydisk.img /1/2/f2 /1/hl2
# echo "\n ---------- \n"
# ./ext2_ls emptydisk.img /
# echo "\n ---------- \n"
# ./readimage emptydisk.img
# echo "\n ---------- \n"
# ./ext2_rm_bonus emptydisk.img -r /1
# echo "\n ---------- \n"
# ./readimage emptydisk.img
# rm_bonus

cp /Users/javiewang/Desktop/emptydisk.img /Users/javiewang/Desktop/CSCC69A3/emptydisk.img
./ext2_mkdir emptydisk.img /dir1
./ext2_mkdir emptydisk.img /dir1/dir2
./ext2_mkdir emptydisk.img /dir1/dir2/dir3
./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /file1
./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /dir1/file2
./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /dir1/dir2/file3
./ext2_ln emptydisk.img -s /dir1/file2 /slink1
./ext2_ln emptydisk.img /dir1/file2 /hlink1
./ext2_ln emptydisk.img -s /dir1/dir2/file3 /dir1/slink2
./ext2_ln emptydisk.img /dir1/dir2/file3 /dir1/hlink2
./ext2_ln emptydisk.img -s /file1 /dir1/dir2/slink3
./ext2_ln emptydisk.img -s /file1 /largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelink
./ext2_ln emptydisk.img /file1 /dir1/dir2/hlink3
./ext2_cp emptydisk.img /Users/javiewang/Desktop/large.txt /dir1/dir2/large.txt
echo "\n---------------------\n"
./readimage emptydisk.img
echo "\n---------------------\n"
./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /dir1/dir2/dir3/largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename0.txt
./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /dir1/dir2/dir3/largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename1.txt
./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /dir1/dir2/dir3/largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename2.txt
./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /dir1/dir2/dir3/largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename3.txt
./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /dir1/dir2/dir3/largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename4.txt
./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /dir1/dir2/dir3/largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename5.txt
./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /dir1/dir2/dir3/largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename6.txt
./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /dir1/dir2/dir3/largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename7.txt
./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /dir1/dir2/dir3/largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename8.txt
./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /dir1/dir2/dir3/largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename9.txt
./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /dir1/dir2/dir3/largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename10.txt
./ext2_cp emptydisk.img /Users/javiewang/Desktop/hello.txt /dir1/dir2/dir3/largenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargenamelargename11.txt
./ext2_ln emptydisk.img /dir1/dir2/large.txt /large
./ext2_ln emptydisk.img /dir1/dir2/large.txt /dir1/large
echo "\n---------------------\n"
./readimage emptydisk.img
echo "\n---------------------\n"
./ext2_rm_bonus emptydisk.img -r /dir1
echo "\n---------------------\n"
./readimage emptydisk.img
echo "\n---------------------\n"