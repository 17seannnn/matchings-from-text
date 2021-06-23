#!/usr/bin/env bash

# compile
gcc -Wall -g ../src/main.c -o ../src/mft
echo ""

# With --help
echo "PARAMTEST --help"
./../src/mft --help
echo ""

# With -h
echo "PARAMTEST -h"
./../src/mft -h
echo ""

# With -q
echo "PARAMTEST -q"
cat linepos.txt | ./../src/mft -q '?orem'
echo ""

# With --quiet
echo "PARAMTEST --quiet"
cat linepos.txt | ./../src/mft --quiet '?orem'
echo ""

# With -c
echo "PARAMTEST -c"
cat any_cases.txt | ./../src/mft -c '?orem'
echo ""

# With --any-cases
echo "PARAMTEST --any-cases"
cat any_cases.txt | ./../src/mft --any-cases '?orem'
echo ""

# With -f and -p
echo "PARAMTEST -f -p"
./../src/mft -f text.txt -p 'lorem'
echo ""

# With --file and --pattern
echo "PARAMTEST --file --pattern"
./../src/mft --file text.txt --pattern 'lorem'
echo ""

# Check backslash
echo "CHARACTERSTEST \\"
./../src/mft -f linepos.txt -p '\?' '\*' '\'
echo ""

# Check full text
echo "FULLTEXTTEST"
./../src/mft -f linepos.txt -p '*'
echo ""

# Check lorem
echo "LOREMTEST"
./../src/mft -f text.txt -p 'lorem'
echo ""
