grammar myn;

program: declaration*;

declaration: function_declaration | class_declaration;

function_declaration: 'fun' identifier '(' parameters? ')' '{' statement* '}';

class_declaration: 'class' identifier '(' identifier ')' '{' class_member* '}';

class_member: 'public:' class_function_declaration* variable_declaration*
            | 'private:' variable_declaration*;

class_function_declaration: 'fun' identifier '(' parameters? ')' '{' statement* '}';

variable_declaration: type identifier '=' expression ';';

parameters: parameter (',' parameter)*;
parameter: type identifier;

statement: variable_declaration
         | expression_statement
         | if_statement
         | while_statement
         | for_statement
         | return_statement
         | output_statement
         | block;

expression_statement: expression ';';

if_statement: 'if' '(' expression ')' block ('else' block)?;

while_statement: 'while' '(' expression ')' block;

for_statement: 'for' '(' for_init_statement ';' expression ';' expression ')' block;
for_init_statement: variable_declaration | expression_statement;

return_statement: 'return' expression ';';

output_statement: 'output' '(' expression ')' ';';

block: '{' statement* '}';

// Fixed Expressions without Left Recursion
expression: primary_expression (binary_operator primary_expression)*;

primary_expression: assignment
                  | function_call
                  | literal
                  | identifier
                  | array;

assignment: identifier '=' expression;

function_call: identifier '(' arguments? ')';
arguments: expression (',' expression)*;

binary_operator: '+' | '-' | '*' | '/' | '==' | '!=' | '<' | '>' | '<=' | '>=' | '&&' | '||';

type: 'int' | 'float' | 'string' | 'bool' | identifier;

identifier: LETTER (LETTER | DIGIT | '_')*;

literal: integer_literal | float_literal | string_literal | boolean_literal | 'NULL';

integer_literal: DIGIT+;
float_literal: DIGIT+ '.' DIGIT+;
string_literal: '\'' CHARACTER* '\'';
boolean_literal: 'true' | 'false';

array: '[' expression (',' expression)* ']';

DIGIT: '0'..'9';
LETTER: 'a'..'z' | 'A'..'Z';
CHARACTER: ~'\''; // any valid character except single quote
