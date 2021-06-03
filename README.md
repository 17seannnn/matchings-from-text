# **MFT** - *matches from text* v1.02

## How use it?

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

- [ ] If the pattern isn't present at the beginning of the word,
      it can't be recognized
- [ ] Find matches with any number of patterns
- [ ] Add comments in the code
- [x] The pattern can be smaller than the string it is compared to
- [x] More memory for text
- [x] Use compile-time constants for number of memory
