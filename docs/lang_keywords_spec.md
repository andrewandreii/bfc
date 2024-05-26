- [Control Flow](#control-flow)
  - [`if` and `if0`/`if else`](#if-and-if0if-else)
  - [`while` and `while0`](#while-and-while0)
  - [`switch` (?)](#switch-)
- [Memory structure](#memory-structure)
  - [`struct`](#struct)
  - [`proc`](#proc)
  - [`ref` and preprocessor directives](#ref-and-preprocessor-directives)


## Control Flow

### `if` and `if0`/`if else`
// TODO: add comparison
1) `if` (preserve argument):
   - temp0 required for preserve
   - CG: `var[[temp0+var-]if_body]temp0[var+temp0-]`
2) `if` (dont preserve):
   - no temp required
   - CG: `var[[-]if_body]`
3) `if0` / `if else` (preseerve argument)
   - temp0, temp1 for preserve and else branch
   - CG: `var[temp0+temp1+var-]temp0[var+temp0-]` (preserve argument)
         `temp0+temp1[temp0-temp1[-]if_branch]temp0[-else_branch]`
4) `if0` / `if else` (don't preserve)
   - temp1 for else branch
   - CG: `temp0+var[temp0-var[-]if_branch]temp0[-else_branch]`

mut - don't preserve; 
t0 - temp for preserving; 
t1 - temp for else.

| `if` | mut `if` | `if0` | mut `if0` |
| --- | --- | --- | --- |
| t0 | none | t0, t1 | t1 |

### `while` and `while0`
The simplest form of loop. Generated code is mostly: `[code]`

1) `while`
   - CG: `var[]`
2) `while0`
   - gets translated to:
```
if0 var
   while temp0
      // ... code ...
      if var
         dec temp0
      end
   end
end
```

### `switch` (?)
Gets converted to `if0`s
```
dec var, case1
if0 var
   // ... code ...
else
   dec var, case2
   if0 var
      // ... code ...
   else
      // ... and so on ...
   end
end
```


## Memory structure

### `struct`
**Syntax**:
- `struct optional_name(args+default_vaalues)[optional_length]`
or
- `struct optional_name(args+default_values)`, which defines an infinite struct.

This keyword defines an array of "structures". It's the most common way of keeping track of information in brainfuck. For example:
```
struct vars(x, y, z)[1]
struct elements(a, b)
```
Defines a few variables `x`, `y` and `z` at the start of the memory and then an infinite list of `a`-`b` pairs. The memory will look like this: `| x | y | z | a | b | a | b | ... `

For obvious reasons, we cannot define more than one infinite struct per process (and we can't define any other structs after an infinite one either).

### `proc`
**Syntax**:
- `proc optional_name(args) -> return_var` or
- `proc optional_name(args) -> (return_vars)`

This is only useful when splitting a program into more files. If using normal brainfuck memory layout, you have to account for struct length in the calling process (ie. the calling process cannot allocate for itself infinite memory). But, if you're using the 2D memory feature, the length of the structs doesn't matter.

When calling a process we use `call` and the arguments we provide get copied either in the memory space of the callee. The same happens to the return values.

`proc` doesn't define variables, they have to be defined by a struct later, otherwise the compiler doesn't know where to place them.

### `ref` and preprocessor directives
**Syntax**:
```
[[directives]]
ref vars
```
Some useful directives are:
- `useif` - the variables will be used for any conditions;
- `usea` - the variables will be used for arithmetic simplifications;
- `usecpy` - the variables will be used when you want to copy things from one cell to another;
- `mut` - the variables specified can be changed during `if else` or other destructive constructs. This simplifies the bf code, but it might be hard to keep track of;
- more to be added.