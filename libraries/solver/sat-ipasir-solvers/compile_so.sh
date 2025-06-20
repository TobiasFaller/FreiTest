#!/bin/bash

# Author: Johannes Gramsch (2021)


echo "This scripts compiles a shared library object (.so) from given static library (.a)"

if ! test -f "$1"; then
	echo "File $1 not found"
	echo "Usage: ./compile_so.sh static_library_file.a"
	exit
fi

TEMP_DIR="temp_dir/"
DIR_NAME=$(dirname $1)
FILE_NAME=$(basename $1)
LIB_NAME=$(basename $1 .a)
SO_NAME=$LIB_NAME.so

# Create temporary directory
mkdir $TEMP_DIR
cp $1 $TEMP_DIR
cd $TEMP_DIR

# Extract archive

ar xv $FILE_NAME

# Compile shared object

if ! gcc -shared *.o -o $SO_NAME; then
	echo "Error during compiling"
else
	# Check symbols in shared object
	# nm -D $SO_NAME

	mv $SO_NAME ../$DIR_NAME
	echo "Compiled $DIR_NAME/$SO_NAME"
fi



# Cleanup

cd ..
rm -r $TEMP_DIR




