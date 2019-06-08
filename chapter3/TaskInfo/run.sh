make clean
make

echo "-*- test task_info module -*-"
sudo insmod task_info.ko
echo "1998" > /proc/pid
cat /proc/pid
echo "18442" > /proc/pid
cat /proc/pid
echo "20019" > /proc/pid
cat /proc/pid
sudo rmmod task_info

# make clean
