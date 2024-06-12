# BFC Compiler (name subject to change)

This is a transpiler for the language that outputs bf code.

**Bfc** is a compiled language that is meant to be readable and scalable, while also following all of brainf*ck's limitations.

The language is assembly-like with some higher-level concepts implemented:

```bfc
struct (a, b)

read a
read b
while a
    dec a
    dec b
    if0 a
        if0 b
            write "Equal"
        else
            write "Not equal"
        end
    end
end
```

This is a simple program that reads 2 numbers and checks to see if they're equal.

The same program in bf would've been a headache to write even though this is a simple task. While you don't have a simple way of writing this in bfc either (you can't just write if a == b), it's less of a headache and it follows bf's design patterns.

## Installation

1. Clone the repo
```sh
git clone <url>
cd <folder>
```

2. Run configure
```sh
./configure.sh
```

3. Run make
```sh
make install
```

And done. Now you have access to a high level language that compiles to brainfuck. Make sure you get a bf interpreter to actually run your programs.

## Usage

TODO

## Contributing

I'd appreciate it a lot if someone takes interest into this project so you're welcome to contribute and I'll look over the pull request asap.

1. Fork the repository.
2. Create a new branch: `git checkout -b <feature-name>`.
3. Make your changes.
4. Push your branch: `git push origin <feature-name>`.
5. Create a pull request.

## Licence

See [this file](LICENCE).

## Why?

### Simple memory managment

A common design pattern in bf is to structure your memory into groups of cells and bfc provides an easy way to keep track of the groupings.

Suppose you have a list of numbers and you want to be able to compare any two adjecent elements. This would be done by having, for each element, one cell that remembers the name and possibly three other cells: two used to preserve the two values and another one used to check whether they are 0 or not. This number can be brought down, but the design pattern stands.

| value | value_copy | if_else_var | value2_copy |

The structure above would be repeated along the whole memory.

### Automatically creates temporary variables

One thing that is troublesome in bf, but not in bfc is the temporary variables for ifs, copying values, adding bigger numbers to a cell etc. Bfc is kind enough to create those temporary variables for you, notify when and why they were created and use them without being explicitly told to do so.

### No more writting the same bf templates

An if-else is not an easy task to write in bf. Even if it's not that hard to understand, it's a lot of work and programmers are supposed to be lazy (so I've writted this whole compiler because my fingers got tired of writing all those ifs).

### What times what times what times... is 72?

While not generating the tinies code in bf, the task of giving cells constant values is lifted off your shoulders and into the hands of bfc.

### Which cell am I in again?

No more remebering where you are the 100th time. Bfc moves you through cells, you only need to move through cell groupings.


Well, this is really starting to sound like a comercial, so I will end it here. In the end, this project is just a pasion project and is, indeed, utterly useless.