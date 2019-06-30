#!/bin/bash
index=0
while read line ; do
    lines[$index]="$line"
    index=$(($index+1))
done < ../version
before="${lines[0]}\\.${lines[1]}\\.${lines[2]}"
patch=$((${lines[2]}+1))
after="${lines[0]}\\.${lines[1]}\\.${patch}"
echo "Updated to ${lines[0]}.${lines[1]}.${lines[2]}"
sed -i "s/${before}/${after}/g" ../README.md
sed -i "s/#define PATCH ${lines[2]}/#define PATCH ${patch}/g" ../hmi-display/gva.h
echo "${lines[0]}" > ../version
echo "${lines[1]}" >> ../version
echo "${patch}" >> ../version
