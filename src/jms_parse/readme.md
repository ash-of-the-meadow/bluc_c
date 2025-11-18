# Parser design guidelines / cheatsheet
This document is intended to guide the development of the parser, and to keep general design rules and guidelines all in the same place.

## General Rules
### Parser State
#### Parser-mutating / State-depenmdent functions
1. If a parser function mutates the parser state, explicitly mention it in a doc comment `post-condition` on the headerfile, **no exceptions**.
    1. If it's a static function only in the c file (no header references), then mention it there instead.
1. If a parser function *depends on* a specific state of the parser to work, explicitly mention it in a doc comment `pre-condition` on the headerfile, **no exceptions**.

#### Parser Advancement
3. Only advance the current token if you call out to a function matching the criteria in point #1 or #2
    1. Otherwise, just use `peek` functions to check the state
1. 

## Statement Precedence Table
| Precedence | Description | Subparser Filename |
| ---------- | ----------- | ------------------ |
| 0          | variable declaration  | jms_parser_var |
| 1          | block, i.e. {}        | jms_parser_block |
| 2          | if / else if          | jms_parser_ifElif |
| 3          | expression statements | jms_parser_exprStmt |
| 4          | class definition, i.e. `class` and contents therein | jms_parser_class |
| 5          | return statement      | jms_parser |
| 6          | loop, i.e. `for` / `foreach` / `while` / `do-while` | jms_parser_loop |
| 7          | module, i.e. `module nameHere` | jms_parser_module |
| 8          | invalid start of statement | jms_parser_stmt |