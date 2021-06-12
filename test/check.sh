#!/usr/bin/env bash

# compile
gcc -Wall -g ../src/mft.c -o ../src/mft
echo ""

# With -q
echo "PARAMTEST -q"
cat ../test/linepos.txt | ./../src/mft -q '?orem'
echo ""

# With --quiet
echo "PARAMTEST --quiet"
cat ../test/linepos.txt | ./../src/mft --quiet '?orem'
echo ""

# With --help
echo "PARAMTEST --help"
./../src/mft --help
echo ""

# Check backslash  
echo "CHARACTERSTEST"
cat ../test/linepos.txt | ./../src/mft '\?' '\*' '\'
echo ""

# Check full text
echo "FULLTEXTTEST"
cat ../test/linepos.txt | ./../src/mft '*'
echo ""

# Check lorem
echo "LOREMTEST"
cat ../test/text.txt | ./../src/mft 'lorem'
echo ""
