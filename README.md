# `egg` text transformer
## Usage
```shell
$ egg [filename...|transform...]
```
The `egg` tool will read the string to be transformed from standard input. The transformed string will be written to standard output.

## Example
```shell
$ echo "Hello, world!" | egg "u" # uppercase
HELLO, WORLD!
$ echo "Hello, world!" | egg "l" # lowercase
hello, world!
$ echo "Hello, world!" | egg "{H = G}" # replace H with G
Gello, world!
$ echo "Hello, world!" | egg "{H = G o = a}" # replace H with G and o with a
Gella, warld!
$ echo "Hello, world!" | egg "{H = G o = a}" "u" # replace H with G and o with a, then uppercase
GELLA, WARLD!
$ echo "hello, world!" | egg "[u l]" # every even letter to uppercase, odd to lowercase
HeLlO, WoRlD!
$ echo "hello, world!" | egg leetify.eggscript # grabs the transformations from the file leetify.eggscript
h3110, w0r1d!
$ echo "hello, world!" | egg "xl" # removes all 'l' characters
heo, word!
```
## Available transformations
- `u`: Converts each letter to uppercase.
- `l`: Converts each letter to lowercase.
- `r`: Reverses the string.
- `C`: Capitalize the first letter of each word.
- `d`: Duplicates the string.
- `s`: Removes all whitespace.
- `t`: Trims the string.
- `j`: Replaces all whitespace with a '_'.
- `e`: Escapes the string.
- `n`: Unescapes the string.
- `i`: Toggles the case of each letter.
- `-`: Removes the last character from the string.
- `b`: Encodes the string in base64.
- `B`: Decodes the string from base64.
- `h`: Encodes the string in hex.
- `H`: Decodes the string from hex.
- `^`: Runs the string through a simple XOR cipher and returns the result as a hex encoded string.
- `c`: Calculates the crc32 checksum of the string and returns the result as a hex encoded string.
- `a<char|string>`: Adds the specified character or string to the end of the string.
- `p<char|string>`: Adds the specified character or string to the beginning of the string.
- `x<char|string>`: Removes the specified character(s) from the string.
- `E(<transform...>)`: Executes the given transformations for each character in the string seperately.
- `'file'`: Executes all transformations in the specified file (called `file.eggscript`). Can be a path.
- `{<from: char> = <to: char>}`: Replaces all instances of the specified character with the specified character. The replacement is case-sensitive. For example, `{H = G}` will replace all instances of `H` with `G`, but not `h`. You can also use this to replace multiple characters at once, like `{H = G o = a}`.
- `[<transform...>]`: Applies the specified transformations to each character in the string. The transformations will be applied in the order they are listed in the brackets. For example, `[u l]` will apply the `u` transformation to every even character and the `l` transformation to every odd character. This is useful for creating alternating patterns.
