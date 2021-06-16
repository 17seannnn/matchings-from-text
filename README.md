
# **MFT** - *matches from text* v1.12

## How to use it?

```bash
./mft -[PARAM] '[Pattern1]' '[Pattern2]'...
```
Then you can enter the string
or do something like this:
```bash
cat ../test/text.txt | ./mft 'lorem' '\?' 'u*a'
cat ../test/linepos.txt | ./mft '?orem' '\'
```

### Patterns
Special characters for patterns:

'**\***' = any characters of any length

'**?**' = any character, but cannot be empty

'**\\**' = place it before \* and ? for finding star and question mark

Examples:
``` bash
'?orem' 'b*ye' 'questions\?' '\*stars\*' '\'
```

### Params
-q, --quiet = show matches without line and position

--help      = show help

## Install

```bash
git clone https://github.com/17sean/mft.git
cd mft/src
gcc mft.c -o mft
```

## Tasks

- [x] Add comments in the code
- [x] Update README: add block about patterns
- [x] Add --help param
- [x] Find matches with any number of patterns
- [x] Add -q param to show words without line and pos
- [x] Show line number and position
- [x] Add \ for finding '?', '\*' and '\\'
- [x] If the pattern isn't present at the beginning of the word, it can't be recognized
- [x] The pattern can be smaller than the string it is compared to
- [x] More memory for text
- [x] Use compile-time constants for number of memory
