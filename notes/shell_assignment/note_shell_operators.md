shell concepts: for exec, pipes, redirects ( $ echo hi > text.txt )

## Using bitwise operators to store flags in integers

FLAG1 = 1 
FLAG2 = 2
FLAG3 = 4
FLAG4 = 8

do_thing ( FLAG2 | FLAG4);
FLAG2 | FLAG4 = 10

if(flags & FLAG2){
    flag2 is set
}

### Boolean vs Bitwise

a = 1
b = 1
c = 4

a && b =1
a & b = 1
a && c = 1// both non-zer0
a & c  = 0000
    a   = 0001b;
    c   = 0100b;
    a&c = 0000b;

### Extra notes
`$ tac` -> reverses order of words


