#!/bin/bash


head -n 2 "$1"

for i in "$@"; do
	line=$(grep -n '%' "$i" | cut -f 1 -d ':')
	line=$(($line - 1))
	head -n $line "$i" | tail -n +3
done

echo '%'

for i in "$@"; do
	line=$(grep -n '%' "$i" | cut -f 1 -d ':')
	line=$(($line + 1))
	tail -n +$line "$i" | head -n -1
done

tail -n 1 "$1"
