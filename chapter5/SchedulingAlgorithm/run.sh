make clean


make fcfs
make sjf
make priority
make rr
make priority_rr

echo "---------fcfs-------------"
./fcfs schedule.txt

echo "---------sjf-------------"
./sjf schedule.txt

echo "---------priority-------------"
./priority schedule.txt

echo "---------rr-------------"
./rr schedule.txt

echo "---------priority_rr-------------"
./priority_rr schedule.txt
