# Expression design guidelines / cheatsheet
This document is intended to guide the development of expressions, and to keep general design rules and guidelines all in the same place.

## Expression Precedence Table
| Precedence | Tokens | Description | Subparser Filename |
| ---------- | ------ | ----------- | ------------------ |
| 1   | ++ -- | suffix/postfix increment and decrement | jms_parser_basicUnary |
| ..1 | ()    | grouping (parenthesis)                 | ..jms_parser_basicUnary |
| ..1 | []    | brackets/index                         | ..jms_parser_basicUnary |
| ..1 | .     | struct and union member access, thru pointer or variable | ..jms_parser_dotOperator |
| 2   | *     | 