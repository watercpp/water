#! /bin/sh
# utf-create.sh something.json (must be utf-8)
# outputs something utf-16 little-endian.json .... and so on
name=${1//.json/}
iconv -f utf-8 -t utf-16le "$1" > "$name utf-16 little-endian.json"
iconv -f utf-8 -t utf-16be "$1" > "$name utf-16 big-endian.json"
iconv -f utf-8 -t utf-16 "$1" > "$name utf-16 bom.json"
iconv -f utf-8 -t utf-32le "$1" > "$name utf-32 little-endian.json"
iconv -f utf-8 -t utf-32be "$1" > "$name utf-32 big-endian.json"
iconv -f utf-8 -t utf-32 "$1" > "$name utf-32 bom.json"