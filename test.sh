#./readimage twolevel.img

# ls
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

# cp
# ./readimage emptydisk.img
# echo "\n---------------------\n"
# # ./ext2_cp emptydisk.img /Users/javiewang/Desktop/large.txt /large.txt
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

# rm



# mkdir + ls
echo "\n----------mkdir--------\n"
./ext2_mkdir emptydisk.img /123
echo "\n----------ls-----------\n"
./ext2_ls emptydisk.img /
echo "\n---------readimage----\n"
./readimage emptydisk.img
echo "\n---------test---------\n"
./test emptydisk.img /
echo "\n---------test---------\n"
./test emptydisk.img /123
echo "\n---------test---------\n"
./test emptydisk.img /lost+found
