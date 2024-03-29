# OS-from-Assembly
There are many projects. One which simulates the binary file is completed.

## Binary Simulator
### Running
1. Compilation has to be done via the following command:
   ```
   g++ -o simulator.out CPU.cpp --std=c++20
   ```
   
2. To run the instructions, follow the following syntax:
   ```
   ./simulator.out instruction_file_loc [memory_dump_loc] [memory_input_loc]
   ```
   
### I/O Redirections
- `stdout` and `stdin` have no use.
- To see the debug output, pass the `-DDEBUG_MODE` flag to compiler while compiling. Debug output is automatically redirected to `stderr`.
- Debug output have the following format:
  ```
  <binary instruction to execute> <A after execution> <D afetr execution> <data at address value of A after execution> <PC value after execution>
  ...
  ```
- In case of any invalid condition, the program will abort with a call to `throw std::runtime_error()`.

### File Format
Both memory and instruction files have one instruction per line, with each instruction in binary format. For example, we can have the following instruction file:
```
0000 0000 0000 0000
1111 1100 0001 0000
0000 0000 0000 0001
1111 0000 1001 0000
0000 0000 0000 0000
1110 0011 0000 1000
1110 1110 1010 0000
1110 1010 1000 0111
```
and the following data file:
```
0000 0000 0000 0001
0000 0000 0000 0010
```

### Special Cases
- Following instruction is used to define a nop operation: `0xFFFF`
- Also, when `PC` is set to `0xFFFF`, the program finishes. In the above example, copy last two lines from instructions to set `PC` to $65535$.

### Semantic Special Cases
Consider the following command:
```
AM = M + 1
```
In above command, the result is calculated first and set to both `A` and `M` simultaneously. Thus, there is no dependency between `A` and `M` on lhs.

## Assembler
This is second project. It converts a valid assembly program to corresponding binary output.

### Running
1. Compilation has to be done via the following command:
   ```{bash}
   g++ -o assembler.out *.cpp
   ```
2. To convert the assembly program to binary, do the following:
   ```{bash}
   ./assembler.out <DFA file> <input_assembly_location> <output_file_location>
   ```

### I/O Redirections
`stdout` and `stdin` have no use. Debug output is automatically redirected to `stderr`.
In case of any error, appropriate error message will be printed and is followed by a call to `exit(-1);`.
If everything is correct, then the `stderr` will contain memory locations and jump locations of new symbols defined by the user.

### File Format
1. `DFA.txt` file should be changed with care!.
2. `<input assembly location>` file should have a new line character at the end for the conversion process to be successful without error, assuming that the code is correct.
3. You should use the following command to terminate the simulator (Setting PC to 65535)
   ```
   A=-1
   0;JMP
   ```

## Virtual Machine Translator
This is the third project. It converts a valid bytecode generated from compiler to corresponding assembly output.

### Running
1. Compilation has to be done via the following command:
   ```{bash}
   g++ -o translator.out *.cpp --std=c++17
   ```
2. To convert the given bytecode to assembly, use the following:
   ```{bash}
   ./translator.out <DFA file> <Grammar file> <input_vm_file_location> <output_assembly_location>
   ```

### I/O Redirections
`stdout` and `stdin` have no use. Debug output is automatically redirected to `stderr`.
In case of any error, appropriate error message will be printed and is followed by a call to `exit(-1);`.
If everything is fine, then the `stderr` will contain parser table construction, parsing steps, parse tree output, Abstract Syntax Tree, Type check errors and a success message for assembly dump.

### File Format
#### File level checks
1. There should be Main.main.
2. `Main.main` accepts `0` arguments.
3. There should not be any function named `Sys.init`.
4. Function names should not be repeated.
5. Calls must match existing function names.
6. Calls must match exact number of arguments in the definition.
7. Forward function calls are allowed.

#### Per function checks
1. Check if label names are not repeated inside a local function.
2. Check if calls to jump are made to existing labels.
3. JUMP labels should not start with ["ALU_COMPARE_", "FUNCTION_CALL_"].
4. Two pass required.

#### Stack checks
1. Only `0` and `1` are allowed with `pointer`.
2. Only integers allowed with `temp` are `0-7`.
3. `pop` is not allowed with `constant`.

#### Not Handled Yet
- `pointer` datatype in assembly generation.
- Multiple files as input.


## Compiler
This is the fourth project. It takes multiple files as input and generates a valid bytecode.

### Running
1. Compilation has to be done via the following command:
   ```{bash}
   g++ -o compiler.out *.cpp --std=c++17
   ```
2. To convert the given file to bytecode, use the following:
   ```{bash}
   ./compiler.out <DFA file> <Grammar file> <input_file_location> <output_bytecode_location>
   ```
3. To convert the given files to bytecode, use the following:
   ```{bash}
   NOT IMPLEMENTED YET
   ```

### I/O Redirections
`stdout` and `stdin` have no use. Debug output is automatically redirected to `stderr`.
There are two exit mechanisms in the code:
1. `exit(-1)` is used when the CLI arguments are not correct or the parse table is not constructed successfully.
2. `assert(false)` is used when there are logical errors in the code - Both compiler or user.

If everything is fine, then the `stderr` will contain parser table construction, parsing steps, parse tree output, Abstract Syntax Tree, Type check errors and a success message for assembly dump.

### Code Checks
#### Naming Checks
1. No two classes have same name.
2. No two members in a class can have same name. The member names should not match existing class names.
3. A local variable name and parameter name inside function can't be a class name, member name in existing class, same name.
4. Types of class variable, function returns, local variables and parameters should be valid.
5. Function body - Identifier usages should refer to existing valid names.
6. There should be a class named `Main`. This class should have a function named `main` with no arguments. Other classes can have `main` but that is not our concern.
7. There should not be any member named `init` inside `Sys` class. This is reserved for the entry point of the program inside bytecode.
   - Either `Sys` class should not be present or `init` should not be a function inside it.

#### Member Access rules

The following table is applicable for `X` in `X` or `X.Y` or `X()` or `X.Y()`.

| Case | User Defined Type | Class (self) function | Class (self) vars/function locals/params |
| ---- | ----------------- | --------------------- | ---------------------------------------- |
| `X` | NO | NO | YES |
| `X.Y` | YES | NO | YES |
| `X()` | NO | YES | NO |
| `X.Y()` | YES | NO | YES |

The following table is applicable for `Y` in `X.Y` or `X.Y()`.

| Case | User Defined Type | Class Member Type |
| ---- | ----------------- | --------------------- |
| `X.Y` | NO | Member variable |
| `X.Y()` | NO | Member function |
