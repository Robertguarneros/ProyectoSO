#!/bin/bash
result=$(sudo -S <<< mimara netstat -ap |grep 9050)
echo $result

if [ "$result" == "" ];then
	echo "No esta abierto el puerto"
else
	fuser -k -n tcp 9050
	echo "Se ha cerrado el puerto"

fi

