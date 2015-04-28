#!/bin/bash 

if [ "`uname -s`" == "Darwin" ]; then
        LIBEXT="dylib"
else
        LIBEXT="so"
fi

for fmod in `ls -1 /usr/lib/libXnVFeatures*$LIBEXT`
do
	niReg -u $fmod
done
for hmod in `ls -1 /usr/lib/libXnVHandGenerator*$LIBEXT`
do
	niReg -u $hmod
done

rm /usr/lib/libXnVNite*$LIBEXT
rm /usr/lib/libXnVCNITE*$LIBEXT
rm /usr/lib/libXnVNITE.jni*$LIBEXT
rm /usr/lib/libXnVFeatures*$LIBEXT
rm /usr/lib/libXnVHandGenerator*$LIBEXT
rm -rf /usr/bin/XnVSceneServer*
rm -rf /usr/etc/primesense/Features*
rm -rf /usr/etc/primesense/Hands*
rm -rf /usr/include/nite/
rm /usr/share/java/com.primesense.NITE.jar

if [ -f /usr/bin/gmcs ]
then
	if [ -e /usr/etc/primesense/XnVNITE.net.dll.list ]
	then
		for netdll in `cat /usr/etc/primesense/XnVNITE.net.dll.list`
		do
			netdll=`echo $netdll | sed "s/\.dll//"`
			gacutil -u $netdll
		done
		rm -rf /usr/etc/primesense/XnVNITE.net.dll.list
	fi
fi

echo "Done!"
