make clean
make
./vm addresses.txt
#compare the output and groudtruth
echo "------------------------"
echo "diff correct.txt output.txt"
echo "the difference:"
diff correct.txt output.txt
