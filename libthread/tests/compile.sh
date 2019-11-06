set -v
g++ --shared -fPIC -I ../../stack ../../stack/*.cpp -o stack.so -g3 -O3 || exit
g++ --shared -fPIC ../../include/println.cpp -o println.so -g3 -O3 || exit
g++ --shared -fPIC -I ../../include -I ../../stack -I ../../thread ../../thread/*.cpp stack.so println.so -o thread.so -Wl,-rpath . -g3 -O3 || exit
g++ -I ../../include -I ../../stack -I ../../thread single_thread.cpp thread.so println.so -o single_thread -Wl,-rpath . -g3 -O3 || exit

./single_thread || exit

#gdb --quiet -ex "handle SIGCONT pass print nostop" -ex "handle SIGSTOP pass print nostop" -ex "set follow-fork-mode child" -ex r single_thread