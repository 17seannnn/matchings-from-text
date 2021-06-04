# **MFT** - *matches from text* v1.03

## How to use it?

```bash
./mft [Pattern]
```
Then you can enter the string
or do something like this:
```bash
cat ../test/text.txt | ./mft lorem
```

## Install

```bash
git clone https://github.com/17sean/mft.git
cd mft/src
gcc mft.c -o mft
```

## Tasks

- [ ] Find matches with any number of patterns
- [ ] Show line number and position
- [ ] Add \ for finding '?' and '\*'
- [ ] Add comments in the code
- [ ] Update README: add block about patterns
- [x] If the pattern isn't present at the beginning of the word, it can't be recognized
- [x] The pattern can be smaller than the string it is compared to
- [x] More memory for text
- [x] Use compile-time constants for number of memory
