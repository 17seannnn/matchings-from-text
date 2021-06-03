# **MFT** - *matches from text*

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

- [ ] The pattern can be smaller than the string it is compared to
- [ ] Find matches with any number of patterns
- [ ] More memory for text
- [ ] Use compile-time constants for number of memory
