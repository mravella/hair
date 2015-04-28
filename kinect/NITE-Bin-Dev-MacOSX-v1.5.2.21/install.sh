#!/bin/bash -e

if [ "`uname -s`" == "Darwin" ]; then
        LIBEXT="dylib"
else
        LIBEXT="so"
fi

printf "Installing NITE\n"
printf "***************\n\n"

printf "Copying shared libraries... "
cp Bin/libXnVNite*$LIBEXT /usr/lib
cp Bin/libXnVCNITE*$LIBEXT /usr/lib
cp Bin/libXnVNITE.jni*$LIBEXT /usr/lib
printf "OK\n"

# If samples exist then this is a development installation, so headers are needed
if [ -e Samples/Build/Makefile ]
then
	printf "Copying includes... "
	mkdir -p /usr/include/nite
	cp Include/* /usr/include/nite
	printf "OK\n"
fi

printf "Installing java bindings... "
cp Bin/com.primesense.NITE.jar /usr/share/java/
printf "OK\n"

for fdir in `ls -1 | grep Features`
do
	printf "Installing module '$fdir'...\n"
	mkdir -p /usr/etc/primesense/$fdir
	cp -r $fdir/Data/* /usr/etc/primesense/$fdir
	for so in `ls -1 $fdir/Bin/lib*$LIBEXT`
	do
		base=`basename $so`
		printf "Registering module '$base'... "
		cp $so /usr/lib
		niReg /usr/lib/$base /usr/etc/primesense/$fdir
		printf "OK\n"
	done
	for bin in `ls -1 $fdir/Bin | grep XnVSceneServer`
	do
		printf "Copying XnVSceneServer... "
		full=$fdir/Bin/$bin
		cp $full /usr/bin
		chmod +x /usr/bin/$bin
		printf "OK\n"
	done
done
for hdir in `ls -1 | grep Hands`
do
	printf "Installing module '$fdir'\n"
	mkdir -p /usr/etc/primesense/$hdir
	cp -r $hdir/Data/* /usr/etc/primesense/$hdir
	for so in `ls -1 $hdir/Bin/lib*$LIBEXT`
	do
		base=`basename $so`
		printf "registering module '$base'..."
		cp $so /usr/lib
		niReg /usr/lib/$base /usr/etc/primesense/$hdir
		printf "OK\n"
	done
done

if [ -f /usr/bin/gmcs ]
then
	printf "Installing .NET wrappers...\n"
	for net in `ls -1 Bin/*dll`
	do
		gacutil -i $net -package 2.0
		netdll=`basename $net`
		echo $netdll >> /usr/etc/primesense/XnVNITE.net.dll.list
	done
fi

printf "Adding license.. "
LIC_KEY=""
ASK_LIC="1"
while (( "$#" )); do
	case "$1" in
	-l=*)
		ASK_LIC="0"
		LIC_KEY=${1:3}
		;;
	esac
	shift
done
niLicense PrimeSense 0KOIk2JeIBYClPWVnMoRKn5cdY4=
printf "OK\n"

if [ -e Makefile ]
then
	printf "Running make...\n"
	make
fi

printf "\n*** DONE ***\n\n"
