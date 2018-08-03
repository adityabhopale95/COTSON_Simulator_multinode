taskset -c 0 gcc -S -o t3.s -O3 -c -w /home/user/test.i &
taskset -c 1 gcc -S -o t2.s -O2 -c -w /home/user/test.i &
taskset -c 2 gcc -S -o t1.s -O1 -c -w /home/user/test.i &
taskset -c 3 gcc -S -o t0.s -O0 -c -w /home/user/test.i &
wait
