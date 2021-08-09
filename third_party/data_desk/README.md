# Data Desk v1.0.0

## Description

Data Desk is a project utility that parses a simple C-like data description format. Input files in this data description format are parsed to create corresponding abstract syntax trees which represent the information extracted from the files. These abstract syntax trees are then sent to project-specific custom code that is written by the user. This custom code is simply a dynamic library with a few exported functions that are used as callbacks for the parser. Below is a list of the callbacks.

* `DataDeskCustomInitCallback(void)` is called when the parser starts.
* `DataDeskCustomParseCallback(DataDeskNode *root, char *filename)` is called for every top-level construct that is parsed.
* `DataDeskCustomCleanUpCallback(void)` is called before the parser shuts down.

The abstract syntax graph is formed completely by `DataDeskNode` structures. This structure can be found in the `data_desk.h` file.

Data Desk also offers a number of utility functions for introspecting on abstract syntax trees it passes to your custom code. A list of these is in the `data_desk.h` file, which can be included into your custom layer.

## Usage

To use Data Desk, you'll need to do a few things:

1. Get Data Desk
2. Make or get some Data Desk format files (.ds)
3. Make a project-specific custom layer

### Step 1: Get Data Desk

1. Run the command `git clone https://github.com/ryanfleury/data_desk`
2. `cd data_desk`
3. `build` on Windows or `./build.sh` on Mac/Linux

NOTE: The `build.bat` script on Windows expects to find `cl` (MSVC). Your environment should know about this. The easiest way to do this is to use one of the Visual Studio command prompts (titled `x64 Native Tools Command Prompt for VS<version>`, or `x86 Native Tools Command Prompt for VS<version>`). Otherwise, you can call `vcvarsall.bat` in your terminal environment, which is packaged with Visual Studio.

### Step 2: Make or get Data Desk format files (.ds)

Grab an [example](https://github.com/ryanfleury/data_desk/blob/master/examples/) or make your own.

### Step 3: Make a project-specific custom layer

1. An easy way to write the code for this is to check out the custom layer template, located [here](https://github.com/ryanfleury/data_desk/blob/master/examples/custom_layer_template/custom_layer_template.c). Fill out the functions in your custom layer code however you want to. There are some helper functions available in `data_desk.h` that might be useful for you here. This can be dropped into your code and used.

2. To build a custom layer, you just need to build a DLL (or `.so`  file) with the function callbacks you've written as the appropriate exported symbols. `data_desk.h` outlines what symbols are used for each callback.

### Step 4: Run Data Desk

To run Data Desk with your custom layer, you can use the following command template:

`data_desk --custom /path/to/custom/layer /file/to/parse/1 /file/to/parse/2 ...`

## Usage for CMake projects

To use Data Desk in your CMake projects, it's recommended to use the Data Desk repository as a subdirectory in your project (e.g.: git submodule). Then in your `CMakeLists.txt`, you can use it similarly to the following:
```cmake
# Add data_desk to your project: this will also compile the data_desk binary itself.
add_subdirectory("ext/data_desk")

# Compile a custom layer for Data Desk
data_desk_add_custom_library(
    LIBNAME my_custom_data_desk_layer_shared_library
    SRC "path/to/my/custom_library_code.c")

# Add a build step to process given *.ds files with Data Desk
data_desk_invoke(
    LIBNAME my_custom_data_desk_layer_shared_library
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DATASPECS "path/to/my/data_model.ds"
    OUTPUTS "${CMAKE_CURRENT_SOURCE_DIR}/path/to/generated_outputs.c")
```
Make sure you specify all the output files in `data_desk_invoke` correctly, such that CMake will be able to insert the correct build dependencies for your source files, meaning it will reinvoke Data Desk (if you update the `*.ds` file) *before* compiling the outputted source files.

## Data Desk (.ds) File Documentation

A valid Data Desk file is defined as a set of zero or more *Declaration*s, *Struct*s, *Union*s, *Enum*s, *Flags*s, *Const*s, *Procedure Header*s, or *Comment*s. Each of the following sections defines these (and what they are comprised of).

1. [Identifiers](#identifiers)
2. [Keywords](#keywords)
3. [Numeric Constants](#numeric-constants)
4. [String Constants](#string-constants)
5. [Character Constants](#character-constants)
6. [Unary Operators](#unary-operators)
7. [Binary Operators](#binary-operators)
8. [Expressions](#expressions)
9. [Types](#types)
10. [Declarations](#declarations)
11. [Structs](#structs)
12. [Unions](#unions)
13. [Enums](#enums)
14. [Flags](#flags)
15. [Constant Expressions](#constant-expressions)
16. [Procedure Headers](#procedure-headers)
17. [Comments](#comments)
18. [Tags](#tags)

### Identifiers

**Identifier**s are defined as a non-[keyword](#keywords) sequence of characters that begin with either an underscore or an alphabetic character, and contain numeric characters, alphabetic characters, or underscores (similar to C).

### Keywords

There are a number of reserved keywords in Data Desk files that cannot be used as identifiers. Here is a list of them:
 * `struct`: Used for defining [struct](#structs)s.
 * `union`: Used for defining [union](#unions)s.
 * `enum`: Used for defining [enum](#enum)s.
 * `flags`: Used for defining [flags](#flags).
 * `proc`: Used for defining [procedure header](#procedure-headers)s.
 * `_`: Reserved for blank/unused identifiers.

### Numeric Constants

Numeric constants (**Number**s) are defined as a sequence of characters that begin with a numeric character, and contain only numeric characters, periods, or alphabetic characters.

**NOTE**: Data Desk does not guarantee the correctness as defined by programming languages of your numeric constants. For example, the following will be interpreted by Data Desk as a numeric constant: `1.2.3.a.b.c`. Because Data Desk does not do any evaluation of numeric constants, it will not enforce validity of numeric constants.

### String Constants

String constants (**String**s) can be *single-line* or *multi-line*.

A *single-line* string constant is defined similarly to those in C. It begins with a double-quote character, and ends with a *non-escaped* double-quote character. Double-quote characters can be escaped with a backslash.

A *multi-line* string constant is defined as beginning with three double-quote characters (`"""`), and ending with three double-quote characters (`"""`).

### Character Constants

Character constants (**Char**s) are defined almost identically to single-line string constants, but with single-quote beginning and ending characters instead of double-quote characters.

### Unary Operators

Data Desk defines a subset of the unary operators found in C:

 * `-`: Numeric Negation
 * `!`: Boolean Not
 * `~`: Bitwise Negate

### Binary Operators

Data Desk defines a subset of the binary operators found in C. It does not define shorthand assignment operators, like `+=` or `>>=`, because there is no execution in Data Desk. The following binary operators are defined (in order of ascending precedence):

 * `+`: Addition
 * `-`: Subtraction
 * `*`: Multiplication
 * `/`: Division
 * `%`: Modulus
 * `<<`: Left Bitshift
 * `>>`: Right Bitshift
 * `&`: Bitwise And
 * `|`: Bitwise Or
 * `&&`: Boolean And
 * `||`: Boolean Or

### Expressions

An expression (**Expr**) in Data Desk is defined as:
 
**Expr** <- `Identifier`
	| `Number`
	| `String`
	| `Char`
	| `Expr` `Binary Operator` `Expr`

### Types

**Type**s are used in declarations. They are defined as being the following:

 * A group of 0 or more `*` characters, representing the number of layers of indirection.
 * A type name, which can be:
   * Some [**Identifier**](#identifiers) referring to a type name.
   * A [**Struct**](#structs) definition.
   * A [**Union**](#unions) definition.
 * A group of 0 or more array size specifiers, being defined as: `[` `Expr` `]`

### Declarations

**Declaration**s are defined as follows:

`Identifier` `:` `Type` `;`

### Structs

**Struct**s are groups of zero or more declarations. They are defined as:

`Identifier` ` :: struct`
`{`
	Zero or more `Declaration`s, each followed by `,` or `;` characters.
`}`

### Unions

**Union**s are groups of zero or more declarations. They look almost identical to (**Struct**)[#structs]s, but are meant to correspond to `union`s in C, which imply a different memory layout and intended usage. They are defined as:

`Identifier` ` :: union`
`{`
	Zero or more `Declaration`s, each followed by `,` or `;` characters.
`}`

### Enums

**Enum**s are groups of one or more identifiers. They are defined as:

`Identifier`  ` :: enum`
`{`
	One or more `Identifier`s, each followed by `,` or `;` characters.
`}`

When transpiled to C, these will be defined as a normal C `enum`; that is, the first one will be defined as a constant that evaluates to 0, the next to 1, and so on.

### Flags

`Flags`s are groups of one or more identifiers. They are defined as:

`Identifier` ` :: flags`
`{`
	One or more `Identifier`s, each followed by `,` or `;` characters.
`}`

When transpiled to C, these will be defined as several C preprocessor macros that evaluate to unique bits inside of an integral value. These are similar to [Enum](#enums)s, but their purpose is to define unique bits instead of unique integral values for a set of constants.

### Constant Expressions

Constant expressions (**Const**s) are defined as:

`Identifier` `::` `Expression`

### Procedure Headers

Procedure headers (**Procedure Header**s) are defined as:

`Identifier` `::` `proc (` `Declaration`, `Declaration`, ..., `Declaration` `)` `[ -> Type ]`

### Comments

Comments are ignored by the parser. They can be *single-line* or *multi-line*.

*Single-line* comments can be defined with two `/` characters. They are terminated by a newline character.

*Multi-line* comments can be defined with a `/*` pattern. They are terminated by a `*/` pattern. They can also be nested. For example, if there exists the pattern `/*/*`, it will require `*/*/` to terminate.

### Tags

**Struct**s, **Union**s, **Declaration**s (including those within **Struct**s, **Union**s, and **Procedure Header**s), **Const**s, **Identifier**s within **Enum**s or **Flags**, and **Procedure Header**s can be preceded with one or more **Tag**s. A **Tag** is defined as beginning with a `@` character, followed by an **Identifier**, with an optional set of parentheses, with an optional set of comma-separated expressions. These are used to annotate meta-information about various things. They will be passed to custom-layer code.
