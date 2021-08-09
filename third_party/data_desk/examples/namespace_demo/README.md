# Namespace support for Data Desk (WIP)

## Motivation

Namespaces prevent name clashes. By adding them to Data Desk we can provide conflicting definitions of repeated symbols to custom layers.

## Toy example

Imagine that you have a project that uses Data Desk to define some data types that are saved to disk as binary data. Say for instance, that this enumeration is part of your code:

```
EntryType :: enum
{
	EntryType_A,
	EntryType_B,
}
```

Now, let's say that you use Data Desk's `DataDeskFWriteGraphAsC` to output a C representation of that enum:

```
typedef enum EntryType
{
    EntryType_A,
    EntryType_B,
} EntryType;
```

If you decide to write an `EntryType` value to disk as represented in the memory of your running program, you'll write a `0` for `EntryType_A`s and a `1` for `EntryType_B`s.

Let's further imagine that you decide to introduce an `EntryType_Null` enumerand to account for the fact that sometimes you don't
know the type of an Entry. The idiomatic way of representing that information in C would be to use the first enumerand
in the enum definition (the one represented as `0`) for that purpose. You would then modify the original Data Desk definition
to read:

```
EntryType :: enum
{
	EntryType_Null,
	EntryType_A,
	EntryType_B,
}
```

This new definition is incompatible with the original one because the values used to encode types `A` and `B` have shifted after the addition of `Null`. Any saved file that uses the old representation will need to be translated to be valid after the change. This particular example can be easily solved by hand-writing a conversion routine, but Data Desk can save us from writing boilerplate code in this and other, more complex, scenarios.

The main obstacle to perform this task is the name clashing that occurs because both the old and the new definitions use
the same names to refer to incompatible data types. By adding namespaces to Data Desk we can avoid this name
clash. We can, for instance, make a copy of the original `.ds` file and prepend a `namespace` directive to it:

```
namespace v1;
typedef enum EntryType
{
    EntryType_A,
    EntryType_B,
} EntryType;
```

and use it along with the new definition:

```
EntryType :: enum
{
	EntryType_Null,
	EntryType_A,
	EntryType_B,
}
```

The simultaneous availability of both definitions makes easy to write custom code that generates C code that looks like:

```
typedef enum v1_EntryType
{
    v1_EntryType_A,
    v1_EntryType_B,
}
v1_EntryType;

typedef enum EntryType
{
    EntryType_Null,
    EntryType_A,
    EntryType_B,
}
EntryType;

static EntryType convert_v1_EntryType_to_EntryType(v1_EntryType v)
{
    EntryType result = 0;
    switch(v)
    {
        case v1_EntryType_A: result = EntryType_A; break;
        case v1_EntryType_B: result = EntryType_B; break;
        default: assert(!"Illegal value for enum v1_EntryType\n"); break;
    }
    return result;
}
```

## Usage of the `namespace ` directive

All definitions are included inside a namespace. In the absence of explicit namespace directives, they belong to the
global namespace. When a namespace directive (of the form `namespace xxx;`) is provided, all definitions following it
fall into that namespace. Every new `.ds` file starts on the global namespace. To revert to the global namespace explicitly, issue an empty namespace directive (`namespace;`). This syntax does not allow for nested namespaces.

## Changes to `DataDeskNode`

There are four new `DataDeskNode` members that allow to take advantage of namespaces from custom layers:
* `int namespace_index` : Each namespace is paired with an integer value. The global namespace is always identified with the value `0`. The rest of namespaces are given increasing correlative values in order of appearance. All `DataDeskNode`s, regardless of their type, have a valid `namespace_index` value.
* `char *namespace_string` : Nodes whose names can clash (of type **Identifier**, **StructDeclaration**, **UnionDeclaration**, **EnumDeclaration**, **FlagsDeclaration**, **ConstantDefinition**, **ProcedureHeader** and **Declaration**) are also provided with a textual representation of the namespace they belong to.
* `DataDeskNode *namespace_alias_prev` and `DataDeskNode *namespace_alias_next` : Pointers to homonymous nodes on other namespaces. They are sorted so that `alias_prev` nodes will have lower `namespace_index` values and conversely nodes linked through `alias_next` will have higher `namespace index` values.

## Code generation
`DataDeskFWriteGraphAsC` prepends the `namespace_string` of nodes that have it to their names. For example, this
definition:

```
namespace v1;
SOME_CONSTANT :: 16
```

results in:

```
#define v1_SOME_CONSTANT (16)
```
