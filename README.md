# BFC Compiler

This is a transpiler for bfc that outputs bf code.

**Bfc** is a compiled language that is meant to be readable and scalable, while also being limited to brainf\*ck's operations, memory layout etc.

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

Make sure you get a [bf interpreter/debugger](https://github.com/andrewandreii/bf_ncurses_debugger).

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

 - Simple memory managment
 - Automatically creates temporary variables (TODO)
 - No more writting the same bf templates
 - Automatically writing bf code for constants (TODO)
 - An extra abstraction layer: you don't control the exact cell you are in, only the cell grouping
