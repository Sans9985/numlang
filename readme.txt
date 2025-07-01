=== numlang ===

this is basically a (barely readable) language, meant to be minimalistic, but still feature some functionality.

- how to use?
- just download the exe and throw a script file at it (open cmd, then type `nl.exe <script.file>`)

# requirements:
  - windows 10 or 11 (below untested);
  - around 10mb of free ram;
  - roughly 1mb of storage;
  - basic command prompt skills

if you encounter issues, feel free to open an issue.
if you made something, issue #2 is there to share your creations.


below you can find the guide for the language.

# what is what?
a, b, c = registers (float)
[a|b|c] = variants of an operation.
          eg. 0[a|b] can be used as 0a or 0b
          or 1x[c|d] can be used as 1xc or 1xd
          letters in brackets may be substituted by either a number, or a letter.

opcode		what it does
0[ab|:n|;c]	either:
		prints a single character (xx - hex number) to the screen
		or:
		if 2nd char is a semicolon, it will print a register
		(a, b or c, in place of [b]) instead
		after the semicolon, you can also put '*',
		which prints all 3 registers.
		you cannot print only 2 registers in one instruction.
		or:
		prints a string from one of the 4 slots if 2nd char is a colon
1x[n|:ab|_abcd]	stores n in 'x'
		if 2nd char is a colon, treat [ab] as a hex byte
		if 2nd char is an underscore, treat [abcd] as a hex word
2x		loads whatever is in 'x' into 'c'
3cnnnn		jump to character on specified condition '0xnnnn'... (leading zeros are a MUST!)
4oooo		if alone, or `3` before is false, jump to character '0xoooo'.
		(leading zeros are a MUST!)
5ax		increments ([a] = 'i') / decrements ([a] = 'd') 'x' by 1.
6x		subtracts two registers, then stores the result in 'c'
		if [x] is 'a', then a - b -> c
		if [x] is 'b', then b - a -> c
7a		if [a] is 'a': adds the two registers, then stores the result in 'c'
		if [a] is 'm': multiplies the two registers, then stores the result in 'c'
		otherwise, does nothing.
8x		divides two registers, then stores it in 'c' (rounded to 6 digits)
		if [x] is 'a', then a / b -> round(c, 6)
		if [x] is 'b', then b / a -> round(c, 6)
9x		loads 'c' into register 'x' (only 'a' or 'b' is allowed)
c		simply puts the free slots of the stack into `b`
p[x|:aa]	pushes `x` or a constant byte onto the stack
l		loads the highest stack element into `c`
s[a:string]	stores a given string in one of the 4 slots available; marked with 'a'
		16 characters max.
x		no-op (it does nothing)

alternatively, you can put '!' as the first character of the "script"
to enable logging to the console. (conditional jumps [3cnnnn] will always be logged)

note: there is a 64Kbyte (-1) limit on the scripts.
