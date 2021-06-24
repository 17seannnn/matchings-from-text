# **MFT** - *matchings from text* v1.21

## Install
```bash
git clone https://github.com/17sean/matchings-from-text.git
cd matchings-from-text/src
gcc main.c -o mft
```

## How to use it?
### Usage
```bash
./mft -[PARAMs] '[Pattern1]' '[Pattern2]'...
```

### Examples
```bash
./mft -f ../test/linepos.txt -p '\'
./mft -i -f ../test/ignore_case.txt -p '?orem'
./mft -q -f ../test/text.txt -p 'lorem'
cat ../test/text.txt | ./mft 'lorem' '\?' 'u*a'
```

### Params
-h, --help        = show help. <br>
-v, --version     = show version. <br>
-q, --quiet       = show matchings without file name, line and pos. <br>
-i, --ignore-case = ignore uppercase and lowercase. <br>
-qi, -iq          = --quiet and --ignore-case in one param. <br>
-f, --file        = declare files to search. <br>
-p, --pattern     = declare patterns to match.

## Patterns
### Special characters for patterns

'**\***' = any character string of any length. <br>
'**?**' = any character, but can not be empty. <br>
'**\\**' = place it before \* and ? for finding star and question mark.

### Examples
```bash
'?orem' 'b*ye' 'questions\?' '\*stars\*' '\'
```

## Updates

- Search for matchings from any file.
- Add ignore case param.
- Add version params.
- Add short help '-h'.
- Add check.sh for tests.
- Fix bugs.
