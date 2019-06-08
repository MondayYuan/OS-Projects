make clean
make
sudo dmesg -C

echo "-*-test simple module-*-"
sudo insmod simple.ko
dmesg
sudo rmmod simple

echo "-*-test hello module-*-"
sudo insmod hello.ko
cat /proc/hello
sudo rmmod hello

echo "-*-test jiffies module-*-"
sudo insmod jiffies.ko
cat /proc/jiffies
sudo rmmod jiffies

echo "-*-test seconds module-*-"
sudo insmod seconds.ko
cat /proc/seconds
sleep 2
cat /proc/seconds
sleep 1
cat /proc/seconds
sleep 3
cat /proc/seconds
sudo rmmod seconds

make clean
