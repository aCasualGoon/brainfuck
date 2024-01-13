# Brainfuck Interpreter

## Overview
A simple interpreter, compiler, and interactive shell for the Brainfuck programming language.

#### Brainfuck
Brainfuck is an esoteric programming language so it is designed to be "fun", not exactly practical. In Brainfuck you have a tape of character cells, each initialized to zero, and a pointer to a cell. The language has only 8 commands, each represented by a single character:
- `>` increment the pointer
- `<` decrement the pointer
- `+` increment the cell at the pointer
- `-` decrement the cell at the pointer
- `.` output the character signified by the cell at the pointer
- `,` input a character and store it in the cell at the pointer
- `[` jump past the matching `]` if the cell at the pointer is zero
- `]` jump back to the matching `[` if the cell at the pointer is nonzero

The Brainfuck "standard" does not make any further specifications so this interpreter uses the following:
- Any character that is not `>`, `<`, `+`, `-`, `.`, `,`, `[`, or `]` will be treated as a comment.
- The tape is infinite in both directions. This is realized by dynamically allocating memory for the tape. The program won't stop you from allocating infinite memory, it will just keep going until you run out of memory or the operating system kills the process.
- Cell values will overflow from 255 to 0 and underflow from 0 to 255.
- A failure to allocate memory for the tape, input, or the program code will result in program termination.
<!-- - The only syntax error possible is an unmatched `[` or `]`. This will result in program termination. -->

## Compilation and Installation
- To just compile run:
  ```
  make
  ```
- To install (Linux systems only) run:
  ```
  sudo make install
  ```
- To uninstall run:
  ```
  sudo make uninstall
  ```
Compilation and installation require make and gcc to be available in the command line. Alternatively compile brainfuck.c with your C compiler of choice.

## Usage
To show the help page run:
```
brainfuck -h
```
<div style="margin-top: -22.5px; margin-bottom: -5px; margin-left: 10px">or</div>

```
brainfuck --help
```

#### Interpreter
The interpreter can be called from the command line using:
```
brainfuck <input...>
```
Where `<input...>` is a non-empty list of Brainfuck source code that will be concatenated together and interpreted. An element of `<input...>` may be Brainfuck source code directly or a file path to a Brainfuck source code file.

#### Compiler
The compiler can be called from the command line using:
```
brainfuck -c <output> <input...>
```
<div style="margin-top: -22.5px; margin-bottom: -5px; margin-left: 10px">or</div>

```
brainfuck --compile <output> <input...>
```

Where `<output>` is the *required* output executable. `<input...>` works just like when interpreted.
-c or --compile may be any argument as long as they are being succeeded by a valid file name for the output executable.

The compiler requires gcc to be available in the command line.

#### Interactive shell
The interactive shell will be launched if no arguments are provided:
```
brainfuck
```
The interactive shell supports the following additional commands:
- `exit` exit the shell
- `clear` clear the tape
- `help` print this list of commands (+ the normal Brainfuck commands)
The input must exactly match one of these:
✅ "exit"       ❌ " exit"
                    "exit "
                    "++>exit"

## License
This project is licensed under the Mozilla Public License 2.0 - see the [LICENSE.md](LICENSE.md) file for details.
