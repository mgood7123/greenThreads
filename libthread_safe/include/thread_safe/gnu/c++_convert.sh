# copy /usr/include/c++
rm -rf 9
cp -ir /usr/include/c++ .
chmod 766 -R c++
cp -ir /usr/include/x86_64-linux-gnu/c++ .
chmod 766 -R c++
PROCESS="$(readlink -e ./process.sh)"
cd c++
PATH_TO_PROCESS_SH="$(readlink -e ../process.sh)"
find -type f -exec bash -c "$PATH_TO_PROCESS_SH {}" \;