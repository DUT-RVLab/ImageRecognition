#!/bin/bash
################################################################################
# USAGE
# create folders and copy essential files
# 
# ARGS
# $1 parent directory
# $2 appendix
################################################################################

PARENT_DIR='./'
APPENDIX=''

if [ $# -gt 0 ]; 
then
	PARENT_DIR=$1
fi

if [ $# -gt 1 ]; 
then
	APPENDIX=-$2
fi

MASTER_DIR=`echo $PARENT_DIR`data`date +%m%d``echo $APPENDIX`


mkdir `echo $MASTER_DIR`
mkdir `echo $MASTER_DIR`/image
mkdir `echo $MASTER_DIR`/data
mkdir `echo $MASTER_DIR`/script


cp ./integrate_data.sh `echo $MASTER_DIR`/script/
cp ./delete_data.sh `echo $MASTER_DIR`/script/


echo folder created in $MASTER_DIR
