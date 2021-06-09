# **MFT** - *matches from text* v1.09

## How to use it?

```bash
./mft "-param" "[Pattern1]" "[Pattern2]"...
```
Params:
-q || --quiet = show matches without line and position
--help = show help

Then you can enter the string
or do something like this:
```bash
cat ../test/text.txt | ./mft "lorem"
```

## Install

```bash
git clone https://github.com/17sean/mft.git
cd mft/src
gcc mft.c -o mft
```

## Tasks

- [ ] Add comments in the code
- [ ] Update README: add block about patterns

- [ ] Add help param ***partially done***
- [x] Find matches with any number of patterns
- [x] Add -q param to show words without line and pos
- [x] Show line number and position
- [x] Add \ for finding '?', '\*' and '\\'
- [x] If the pattern isn't present at the beginning of the word, it can't be recognized
- [x] The pattern can be smaller than the string it is compared to
- [x] More memory for text
- [x] Use compile-time constants for number of memory
