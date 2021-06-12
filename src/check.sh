#!/usr/bin/env bash

# compile
gcc -Wall -g mft.c -o mft
echo ""

# With -q
echo "PARAMTEST -q"
cat ../test/linepos.txt | ./mft -q '?orem'
echo ""

# With --quiet
echo "PARAMTEST --quiet"
cat ../test/linepos.txt | ./mft --quiet '?orem'
echo ""

# With --help
echo "PARAMTEST --help"
./mft --help
echo ""

# Check backslash  
echo "CHARACTERSTEST"
cat ../test/linepos.txt | ./mft '\?' '\*' '\'
echo ""

# Check full text
echo "FULLTEXTTEST"
cat ../test/linepos.txt | ./mft '*'
echo ""

# Check lorem
echo "LOREMTEST"
cat ../test/text.txt | ./mft 'lorem'
echo ""
