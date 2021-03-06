#/bin/bash

# effectively: sha1sum filename | awk '{ print $1 }' | xxd -r -p | base64 | tee >(base64 -d | xxd -p); sleep 1; echo ""

fileToB64md5=$1
if [ -f "$fileToB64md5" ]; then
	firstMd5=`sha1sum "$fileToB64md5" | awk '{ print $1 }'`
	echo "Initial SHA1:"
	echo "$firstMd5"
	echo ""
	base64encoded=`echo "$firstMd5" | xxd -r -p | base64`
	echo "Base64 Encoded:"
	echo "$base64encoded"
	echo ""
	secondMd5=`echo "$base64encoded" | base64 -d | xxd -p`
	echo "Second (verification) SHA1:"
	echo "$secondMd5"
else
	echo "$fileToB64md5 does not exist"
	exit
fi
