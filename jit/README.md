# An experiment understanding JIT

Based on this [blog](https://eli.thegreenplace.net/2017/adventures-in-jit-compilation-part-1-an-interpreter/),
all credits to the [author](https://github.com/eliben),
recreating it for my understanding

Original Source At: https://github.com/eliben/code-for-blog

|                     | Bf       | Jump Table    |   OpCode   | Opcode Loop Opti   |
|---------------------|----------|---------------|------------|--------------------|
| Factor of 10        |  NA      |   0.5s        |   ~0.3s    |       ~0.3s        |
| Factor of 179424691 |  NA      |   ~7s         |    ~3s     |        ~2s         |
| Mandelbrot          |  NA      |   18s         |    ~7s     |        ~3.5s       |
| Sierpinski          |  NA      |   0.005s      |    0.004s  |        0.003s      |

### Part 1 - The basic interpretor: 

Pretty much the same as given.

Happy to say that I was able to use `getopts` for parsing the cli params

Factor and Mandelbrot didn't complete in my machine, was too slow
Factor didn't complete for a simple value of 10

### Part 2 - Jump Table:

Add a jump table to know which locations the program should jump to next in case of loop

Was able to run Factor for 10 in `0.5s`
Was able to run factor for 179424691 in `~7s`

### Part 3 - Op codes and Optimizations

This section is a big learning to me.

`eliben` talks about translating the program into OpCodes, something like `++++++` will be `OpCode<+, 6>`.

What is interesting to me is this, without exposing to user to more complex operations we are maintaining the same while simplifying it in the interpretor

### Part 4 - More opcodes, optimize loops 

Detect patterns like
- `[-]` Set
- `-<+> and ->+<`

