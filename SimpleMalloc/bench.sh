make
rm ./report/smalloc.txt
echo "[Simple Malloc]" >> ./report/smalloc.txt
echo -e "\n[Thread Num = 1 Malloc Times=100000]" >> ./report/smalloc.txt
./main --malloc_type=0 --thread_num=1 --malloc_times=1000000 \
                                                >> ./report/smalloc.txt
echo -e "\n[Thread Num = 4 Malloc Times=100000]" >> ./report/smalloc.txt
./main --malloc_type=0 --thread_num=4 --malloc_times=1000000 \
                                                >> ./report/smalloc.txt
echo -e "\n[Thread Num = 8 Malloc Times=100000]" >> ./report/smalloc.txt
./main --malloc_type=0 --thread_num=8 --malloc_times=1000000 \
                                                >> ./report/smalloc.txt
echo -e "\n[Thread Num = 16 Malloc Times=100000]" >> ./report/smalloc.txt
./main --malloc_type=0 --thread_num=16 --malloc_times=1000000 \
                                                >> ./report/smalloc.txt
echo -e "\n[Thread Num = 32 Malloc Times=100000]" >> ./report/smalloc.txt
./main --malloc_type=0 --thread_num=32 --malloc_times=1000000 \
                                                >> ./report/smalloc.txt
echo -e "\n\n[Finish Simple Malloc Bench]"

 


