# `egg` text transformer
## Usage
```shell
$ egg [transform...]
```
The `egg` tool will read the string to be transformed from standard input. The transformed string will be written to standard output.

## Example
```shell
$ echo "Hello, world!" | egg "u" # uppercase
HELLO, WORLD!
$ echo "Hello, world!" | egg "l" # lowercase
hello, world!
$ echo "Hello, world!" | egg "{'H' = 'G'}" # replace H with G
Gello, world!
$ echo "Hello, world!" | egg "{'H' = 'G' 'o' = 'a'}" # replace H with G and o with a
Gella, warld!
$ echo "Hello, world!" | egg "{'H' = 'G' 'o' = 'a'}" "u" # replace H with G and o with a, then uppercase
GELLA, WARLD!
$ echo "hello, world!" | egg "[u l]" # every even letter to uppercase, odd to lowercase
HeLlO, WoRlD!
$ echo "hello, world!" | egg "'leetify'" # grabs the transformations from the file leetify.basket
h3110, w0r1d!
$ echo "hello, world!" | egg "xl" # removes all 'l' characters
heo, word!
$ echo "hello, world!" | egg "x'o, world'" # removes the string 'o, world'
hell!
```
## Available transformations
- `u`: Converts each letter to uppercase.
- `l`: Converts each letter to lowercase.
- `r`: Reverses the string.
- `C`: Capitalize the first letter of each word.
- `D`: Decapitalizes the first letter of each word.
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
- `x<char|string>`: Removes all instances of the specified character or string from the string.
- `E(<transform...>)`: Executes the given transformations for each character in the string seperately.
- `'file'`: Executes all transformations in the specified file (called `file.basket`). Can be a path.
- `{<from: string> = <to: string>}`: Replaces all instances of `<from>` with `<to>`. This can be used to replace characters or strings in the input. For example, `{'H' = 'G'}` will replace all instances of `H` with `G`. Multiple replacements can be chained together, such as `{'H' = 'G' 'o' = 'a'}` to replace both `H` and `o` in one go.
- `L<length>`: Limits the string to the specified length. If the string is longer than the specified length, it will be truncated.
- `[<transform...>]`: Applies the specified transformations to each character in the string. The transformations will be applied in the order they are listed in the brackets. For example, `[u l]` will apply the `u` transformation to every even character and the `l` transformation to every odd character. This is useful for creating alternating patterns.
- `@<index><transform>`: Applies the specified transformation only to the character at the specified index. The index is zero-based, so `@0u` will uppercase the first character of the string, while `@1l` will lowercase the second character. If the index is out of bounds, the transformation will be ignored.

### Notes
- `string`s are specified in single quotes, e.g. `'Hello, world!'`.
- `char`s are specified without quotes, e.g. `H`.
- `transform`s are any of the transformations listed above, e.g. `u`, `l`, `r`, etc. Transformations taking arguments (like `a` and `p`, but not `{` or `[`) must be surrounded by parentheses, e.g. `(a'Hello')`, `(p'World')`, `(x'o, world')`.
