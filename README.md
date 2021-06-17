
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
gcc main.c -o mft
```

## Updates

