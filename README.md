# **MFT** - *matches from text* v1.20

## Install

```bash
git clone https://github.com/17sean/mft.git
cd mft/src
gcc main.c -o mft
```

## How to use it?

```bash
./mft -[PARAMs] '[Pattern1]' '[Pattern2]'...
```
Then you can enter the string
or try something like this:
```bash
./mft -f ../test/linepos.txt -p '\'
./mft -c -f ../test/any_cases.txt -p '?orem'
./mft -q -f ../test/text.txt -p 'lorem'
cat ../test/text.txt | ./mft 'lorem' '\?' 'u*a'
```

### Params

-h, --help      = show help <br>
-q, --quiet     = show matches without line and position <br>
-c, --any-cases = do not care about lowercase or uppercase <br>
-qc, -cq        = --quiet and --any-cases in one param <br>
-f, --file      = declare files to search <br>
-p, --pattern   = declare patterns to match

### Patterns

Special characters for patterns:

'**\***' = any characters of any length <br>
'**?**' = any character, but cannot be empty <br>
'**\\**' = place it before \* and ? for finding star and question mark

Examples:
```bash
'?orem' 'b*ye' 'questions\?' '\*stars\*' '\'
```

### Updates

- Search for matches from any file
- Add any cases param
- Add short help '-h'
- Add check.sh for tests
- Fix bugs
