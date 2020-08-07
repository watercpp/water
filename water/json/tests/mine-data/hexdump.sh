#! /bin/sh
# hexdump -ve '1/1 "0x%.2x,"'

for arg in "$@"
do
    echo $arg
    hexdump -ve '1/1 "0x%.2x,"' "$arg"
    echo "\n"
done