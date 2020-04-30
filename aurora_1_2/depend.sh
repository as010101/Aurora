#!/bin/sh

# This script is taken from Peter Miller's excellent paper, 
# "Recursive Make Considered Harmful".

DIR="$1"
shift 1
case "$DIR" in
"" | ".")
g++ -MM -MG "$@" |
sed -e 's@^\(.*\)\.o:@\1.d \1.o:@'
;;
*)
g++ -MM -MG "$@" |
sed -e "s@^\(.*\)\.o:@$DIR/\1.d $DIR/\1.o:@"
;;
esac
