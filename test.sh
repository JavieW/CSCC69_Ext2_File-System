#./readimage twolevel.img

# ls
#echo "-----test root: <<<./ext2_ls twolevel.img  />>>-----"
#./ext2_ls twolevel.img  /

#echo "-----test root with flag: <<<./ext2_ls twolevel.img -a />>>-----"
#./ext2_ls twolevel.img -a /

#echo "-----test afile: <<<./ext2_ls twolevel.img  /afile>>>-----"
#./ext2_ls twolevel.img /afile

#echo "-----test afile, <<<flag does't metter: ./ext2_ls twolevel.img -a /afile>>>-----"
#./ext2_ls twolevel.img -a /afile

#echo "-----test afile, this should fail: <<<./ext2_ls twolevel.img -a /afile/>>>-----"
#./ext2_ls twolevel.img -a /afile/

#echo "-----test level1: <<<./ext2_ls twolevel.img  /level1>>>-----"
#./ext2_ls twolevel.img /level1

#echo "-----test level 1 end with '/': <<<./ext2_ls twolevel.img -a /level1/>>>-----"
#./ext2_ls twolevel.img -a /level1/

#echo "-----test level2: <<<./ext2_ls twolevel.img -a /level1/level2>>>-----"
#./ext2_ls twolevel.img -a /level1/level2

#echo "-----test bfile: <<<./ext2_ls twolevel.img -a /level1/level2/bfile>>>-----"
#./ext2_ls twolevel.img -a /level1/level2/bfile

#echo "-----test bfile, this should fail: <<<./ext2_ls twolevel.img -a /level1/level2/bfile/>>>-----"
#./ext2_ls twolevel.img -a /level1/level2/bfile/

#echo "-----test non-exist path, this should fail: <<<./ext2_ls twolevel.img -a /level1/level2/cfile>>>-----"
#./ext2_ls twolevel.img -a /level1/level2/cfile

#echo "-----test no input image file, this should fail: <<<./ext2_ls twolevel.img>>>-----"
#./ext2_ls twolevel.img

# note: the bfilee.s file in the disk contain an 16 character! below testcase should actually fail
#echo "-----test print a file name, this should work but fail: <<<./ext2_ls onedirectory.img /level1/bfilee.s>>>-----"
#./ext2_ls onedirectory.img /level1/bfilee.s

echo "-----test mkdir at root, this should fail: <<<./ext2_mkdir emptydisk.img />>>-----"
./ext2_mkdir emptydisk.img /
echo "-----test mkdir lost+found, this should fail: <<<./ext2_mkdir emptydisk.img /lost+found>>>-----"
./ext2_mkdir emptydisk.img /lost+found
echo "-----test mkdir hello at root, this should work: <<<./ext2_mkdir emptydisk.img /hello>>>-----"
./ext2_mkdir emptydisk.img /hello
./ext2_ls emptydisk.img /
echo "-----test mkdir hello at root again, this should fail: <<<./ext2_mkdir emptydisk.img /hello/>>>-----"
./ext2_mkdir emptydisk.img /hello/
./ext2_ls emptydisk.img /
echo "-----test mkdir world at non-existing dir, this should fail: <<<./ext2_mkdir emptydisk.img /hi/world>>>-----"
./ext2_mkdir emptydisk.img /hi/world
echo "-----test mkdir world at hello dir, this should work: <<<./ext2_mkdir emptydisk.img /hello/world>>>-----"
./ext2_mkdir emptydisk.img /hello/world
./ext2_ls emptydisk.img /hello
echo "-----test mkdir good at hello dir, this should work: <<<./ext2_mkdir emptydisk.img /hello/good>>>-----"
./ext2_mkdir emptydisk.img /hello/good
./ext2_ls emptydisk.img /hello
echo "-----test mkdir world at file afile, this should work: <<<./ext2_mkdir twolevel.img /afile/world>>>-----"
./ext2_mkdir twolevel.img /afile/world
echo "-----test mkdir . and .. at hello dir, this should fail: <<<./ext2_mkdir emptydisk.img /hello/.(..)>>>-----"
./ext2_mkdir emptydisk.img /hello/.
./ext2_mkdir emptydisk.img /hello/..
echo "-----test mkdir with no input img file, this should fail: <<<./ext2_mkdir /hello/>>>-----"
./ext2_mkdir /hello/
echo "-----test mkdir with no wrong img file, this should fail: <<<./ext2_mkdir haha.img /hello/>>>-----"
./ext2_mkdir haha.img /hello/
