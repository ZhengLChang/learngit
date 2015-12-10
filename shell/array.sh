#!/bin/bash

array_var=(1 2 3 4 5 6)
echo ${array_var[0]}

array_str[0]="test0"
array_str[1]="test1"
array_str[2]="test2"
array_str[3]="test3"
array_str[4]="test4"

echo ${array_str[2]}

index=3
echo ${array_str[$index]}


echo ${array_str[*]}
echo ${array_str[@]}
echo ${#array_str[@]}

#declare -A ass_array
declare -A fruits_value
fruits_value=([apple]='100dollars' [orange]='150 dollars')
echo "Apple costs ${fruits_value[apple]}"
echo ${!fruits_value[*]}
echo ${#fruits_value[*]}










