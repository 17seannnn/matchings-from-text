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

# With --version
echo "PARAMTEST --version"
./../src/mft --version
echo ""

# With -v
echo "PARAMTEST -v"
./../src/mft -v
echo ""

# With -q
echo "PARAMTEST -q"
cat linepos.txt | ./../src/mft -q '?orem'
echo ""

# With --quiet
echo "PARAMTEST --quiet"
cat linepos.txt | ./../src/mft --quiet '?orem'
echo ""

# With -i
echo "PARAMTEST -i"
cat ignore_case.txt | ./../src/mft -i '?orem'
echo ""

# With --ignore-case
echo "PARAMTEST --ignore-case"
cat ignore_case.txt | ./../src/mft --ignore-case '?orem'
echo ""

# With -qi
echo "PARAMTEST -qi"
cat ignore_case.txt | ./../src/mft -qi '?orem'
echo ""

# With -iq
echo "PARAMTEST -iq"
cat ignore_case.txt | ./../src/mft -iq '?orem'
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
