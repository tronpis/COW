//--------------------------------------------
// COW PROGRAMMING LANGUAGE
// by: BigZaphod sean@fifthace.com
// http://www.bigzaphod.org/cow/
// 
// License: Public Domain
//--------------------------------------------
#include <vector>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <cstdint>

typedef std::uint8_t instruction_t;
typedef std::vector<instruction_t> program_t;
typedef std::vector<int> mem_t;

program_t program;
std::vector<int> jump_table;
mem_t memory;
size_t mem_pos = 0;

int register_val;
bool has_register_val = false;

void quit( bool error )
{
    if( error )
    {
        printf( "\nERROR!\n" );
        exit(1);
    }

#ifndef NO_GREETINGS
    printf( "\nDone.\n" );
#endif
    exit(0);
}

int decode_instruction( const char* token )
{
    switch( token[0] )
    {
    case 'm':
        if( token[1] == 'o' && token[2] == 'o' ) return 0;
        if( token[1] == 'O' && token[2] == 'o' ) return 1;
        if( token[1] == 'o' && token[2] == 'O' ) return 2;
        if( token[1] == 'O' && token[2] == 'O' ) return 3;
        break;
    case 'M':
        if( token[1] == 'o' && token[2] == 'o' ) return 4;
        if( token[1] == 'O' && token[2] == 'o' ) return 5;
        if( token[1] == 'o' && token[2] == 'O' ) return 6;
        if( token[1] == 'O' && token[2] == 'O' ) return 7;
        if( token[1] == 'M' && token[2] == 'M' ) return 9;
        break;
    case 'O':
        if( token[1] == 'O' && token[2] == 'O' ) return 8;
        if( token[1] == 'O' && token[2] == 'M' ) return 10;
        break;
    case 'o':
        if( token[1] == 'o' && token[2] == 'm' ) return 11;
        break;
    };

    return -1;
}

void execute_program( long long max_steps )
{
    const instruction_t* prog = program.data();
    const int program_size = (int)program.size();
    int pc = 0;
    long long steps = 0;

#if defined(__GNUC__) || defined(__clang__)
    static void* dispatch[] = {
        &&op_moo, &&op_mOo, &&op_moO, &&op_mOO,
        &&op_Moo, &&op_MOo, &&op_MoO, &&op_MOO,
        &&op_OOO, &&op_MMM, &&op_OOM, &&op_oom
    };
    if( program_size == 0 ) return;
    instruction_t instruction = prog[pc];

    while( pc < program_size )
    {
        if( max_steps > 0 && ++steps > max_steps )
            quit( true );
        goto *dispatch[instruction];

op_moo:
        if( pc == 0 || jump_table[pc] < 0 )
            quit( true );
        pc = jump_table[pc];
        instruction = prog[pc];
        continue;

op_mOo:
        if( mem_pos == 0 )
            quit( true );
        --mem_pos;
        ++pc;
        if( pc < program_size ) instruction = prog[pc];
        continue;

op_moO:
        ++mem_pos;
        if( mem_pos == memory.size() )
            memory.push_back( 0 );
        ++pc;
        if( pc < program_size ) instruction = prog[pc];
        continue;

op_mOO:
        if( memory[mem_pos] == 3 )
            quit( false );
        if( memory[mem_pos] < 0 || memory[mem_pos] > 11 )
            quit( false );
        instruction = (instruction_t)memory[mem_pos];
        continue;

op_Moo:
        if( memory[mem_pos] != 0 )
            printf( "%c", memory[mem_pos] );
        else
        {
            memory[mem_pos] = getchar();
            while( getchar() != '\n' );
        }
        ++pc;
        if( pc < program_size ) instruction = prog[pc];
        continue;

op_MOo:
        memory[mem_pos]--;
        ++pc;
        if( pc < program_size ) instruction = prog[pc];
        continue;

op_MoO:
        memory[mem_pos]++;
        ++pc;
        if( pc < program_size ) instruction = prog[pc];
        continue;

op_MOO:
        if( memory[mem_pos] == 0 )
        {
            if( jump_table[pc] < 0 )
                quit( true );
            pc = jump_table[pc];
        }
        ++pc;
        if( pc < program_size ) instruction = prog[pc];
        continue;

op_OOO:
        memory[mem_pos] = 0;
        ++pc;
        if( pc < program_size ) instruction = prog[pc];
        continue;

op_MMM:
        if( has_register_val )
            memory[mem_pos] = register_val;
        else
            register_val = memory[mem_pos];
        has_register_val = !has_register_val;
        ++pc;
        if( pc < program_size ) instruction = prog[pc];
        continue;

op_OOM:
        printf( "%d\n", memory[mem_pos] );
        ++pc;
        if( pc < program_size ) instruction = prog[pc];
        continue;

op_oom:
        {
            char buf[100];
            int c = 0;
            while( c < (int)sizeof(buf)-1 )
            {
                buf[c] = getchar();
                c++;
                buf[c] = 0;

                if( buf[c-1] == '\n' )
                    break;
            }
            if( c == (int)sizeof(buf) )
                while( getchar() != '\n' );

            memory[mem_pos] = atoi( buf );
            ++pc;
            if( pc < program_size ) instruction = prog[pc];
            continue;
        }
    }
#else
    while( pc < program_size )
    {
        if( max_steps > 0 && ++steps > max_steps )
            quit( true );

        int instruction = prog[pc];
        bool redispatch = true;

        while( redispatch )
        {
            redispatch = false;
            switch( instruction )
            {
            case 0:
                if( pc == 0 || jump_table[pc] < 0 )
                    quit( true );
                pc = jump_table[pc];
                instruction = prog[pc];
                redispatch = true;
                break;
            case 1:
                if( mem_pos == 0 )
                    quit( true );
                --mem_pos;
                ++pc;
                break;
            case 2:
                ++mem_pos;
                if( mem_pos == memory.size() )
                    memory.push_back( 0 );
                ++pc;
                break;
            case 3:
                if( memory[mem_pos] == 3 )
                    quit( false );
                if( memory[mem_pos] < 0 || memory[mem_pos] > 11 )
                    quit( false );
                instruction = memory[mem_pos];
                redispatch = true;
                break;
            case 4:
                if( memory[mem_pos] != 0 )
                    printf( "%c", memory[mem_pos] );
                else
                {
                    memory[mem_pos] = getchar();
                    while( getchar() != '\n' );
                }
                ++pc;
                break;
            case 5:
                memory[mem_pos]--;
                ++pc;
                break;
            case 6:
                memory[mem_pos]++;
                ++pc;
                break;
            case 7:
                if( memory[mem_pos] == 0 )
                {
                    if( jump_table[pc] < 0 )
                        quit( true );
                    pc = jump_table[pc];
                }
                ++pc;
                break;
            case 8:
                memory[mem_pos] = 0;
                ++pc;
                break;
            case 9:
                if( has_register_val )
                    memory[mem_pos] = register_val;
                else
                    register_val = memory[mem_pos];
                has_register_val = !has_register_val;
                ++pc;
                break;
            case 10:
                printf( "%d\n", memory[mem_pos] );
                ++pc;
                break;
            case 11:
                {
                    char buf[100];
                    int c = 0;
                    while( c < (int)sizeof(buf)-1 )
                    {
                        buf[c] = getchar();
                        c++;
                        buf[c] = 0;

                        if( buf[c-1] == '\n' )
                            break;
                    }
                    if( c == (int)sizeof(buf) )
                        while( getchar() != '\n' );

                    memory[mem_pos] = atoi( buf );
                    ++pc;
                    break;
                }
            default:
                quit( false );
            }
        }
    }
#endif
}

int main( int argc, char** argv )
{
    if( argc < 2 )
    {
        printf( "Usage: %s program.cow\n\n", argv[0] );
        exit( 1 );
    }

    FILE* f = fopen( argv[1], "rb" );

    if( f == NULL )
    {
        printf( "Cannot open source file [%s].\n", argv[1] );
        exit( 1 );
    }

    fseek( f, 0, SEEK_END );
    const long size = ftell( f );
    rewind( f );

    std::vector<char> source( size > 0 ? (size_t)size : 0 );
    if( !source.empty() )
    {
        const size_t bytes_read = fread( &source[0], 1, source.size(), f );
        source.resize( bytes_read );
    }

    char buf[3] = {0,0,0};
    for( size_t i = 0; i < source.size(); ++i )
    {
        buf[2] = source[i];
        const int instruction = decode_instruction( buf );

        if( instruction >= 0 )
        {
            program.push_back( (instruction_t)instruction );
            memset( buf, 0, sizeof(buf) );
        }
        else
        {
            buf[0] = buf[1];
            buf[1] = buf[2];
            buf[2] = 0;
        }
    }

    fclose( f );

#ifndef NO_GREETINGS
    printf( "Welcome to COW!\n\nExecuting [%s]...\n\n", argv[1] );
#endif

    jump_table.assign( program.size(), -1 );
    std::vector<size_t> stack;
    for( size_t i = 0; i < program.size(); ++i )
    {
        if( program[i] == 7 )
            stack.push_back( i );
        else if( program[i] == 0 )
        {
            if( stack.empty() )
                quit( true );

            const size_t open = stack.back();
            stack.pop_back();
            jump_table[open] = (int)i;
            jump_table[i] = (int)open;
        }
    }
    if( !stack.empty() )
        quit( true );

    memory.reserve( 1024 );
    memory.push_back( 0 );

    long long max_steps = 0;
    if( const char* max_steps_env = getenv( "COW_MAX_STEPS" ) )
        max_steps = atoll( max_steps_env );

    execute_program( max_steps );
    quit( false );

    return 0;
}
