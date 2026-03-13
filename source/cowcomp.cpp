//--------------------------------------------
// COW PROGRAMMING LANGUAGE COMPILER
// by: BigZaphod sean@fifthace.com
// http://www.bigzaphod.org/cow/
// 
// License: Public Domain
//--------------------------------------------
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstdint>
#include <cstring>
#include <cstdarg>

#define COMPILER	"g++"
#define FLAGS		"-O3 -x c++"
#define NAME_FLAG	"-o "
#define OUTPUT_EXEC "cow.out"
#define OUTPUT_CPP	"cow.out.cpp"


//#define PRETTY(s)	emit( "\t\t\t// %s\n", s );
#define PRETTY(s)	


typedef std::uint8_t instruction_t;
typedef std::vector<instruction_t> mem_t;
mem_t program;
mem_t::iterator prog_pos;
std::string output;

int moocount(0);
int MOOcount(0);


void quit()
{
    printf( "Compile error.  Invalid source code.\n" );
    exit(1);
}

void emit( const char* fmt, ... )
{
    char local[512];
    va_list args;
    va_start( args, fmt );
    int n = vsnprintf( local, sizeof(local), fmt, args );
    va_end( args );

    if( n <= 0 )
        return;

    if( n < (int)sizeof(local) )
    {
        output.append( local, (size_t)n );
        return;
    }

    std::vector<char> dynamic_buf( (size_t)n + 1 );
    va_start( args, fmt );
    vsnprintf( &dynamic_buf[0], dynamic_buf.size(), fmt, args );
    va_end( args );
    output.append( &dynamic_buf[0], (size_t)n );
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

bool compile( int instruction, bool advance )
{
    switch( instruction )
    {
    // moo
    case 0:
        {
            int level = 1;
            int num = MOOcount + 1;
            mem_t::iterator t = prog_pos;

            t--;   // skip past previous command when searching for MOO.
            if( t != program.begin() )
            {
                if((*t) == 7 )
                    num--;

                while( level > 0 )
                {
                    if( t == program.begin() )
                        break;

                    t--;
            
                    if( (*t) == 0 )
                        level++;
                    else
                    if( (*t) == 7 )  // look for MOO
                    {
                        level--;
                        num--;
                    }
                }
            }
            if( level != 0 && advance )
                quit();
            else if( level != 0 )
            {
                emit( "rterr();" );
                break;
            }

            moocount++;
            emit( "goto M%d;", num );
            emit( "m%d:", moocount );
            PRETTY( "moo" );
        }
        break;

    
    // mOo
    case 1:
        emit( "if(p==m.begin()){rterr();}else{p--;}" );
        PRETTY( "mOo" );
        break;

    // moO
    case 2:
        emit( "p++; if(p==m.end()){m.push_back(0);p=m.end();p--;}" );
        PRETTY( "moO" );
        break;
    
    // mOO    
    case 3:
        // I think it should be possible to build a switch statement here and then
        // use the compile function itself to fill in the possibilities.
//        printf( "NOT IMPLEMENTED: mOO\n\n" );
//        quit();
        emit( "switch(*p){" );
        emit( "case 0:{" ); compile( 0, false ); emit( "}break;" );
        emit( "case 1:{" ); compile( 1, false ); emit( "}break;" );
        emit( "case 2:{" ); compile( 2, false ); emit( "}break;" );
        emit( "case 4:{" ); compile( 4, false ); emit( "}break;" );
        emit( "case 5:{" ); compile( 5, false ); emit( "}break;" );
        emit( "case 6:{" ); compile( 6, false ); emit( "}break;" );
        emit( "case 7:{" ); compile( 7, false ); emit( "}break;" );
        emit( "case 8:{" ); compile( 8, false ); emit( "}break;" );
        emit( "case 9:{" ); compile( 9, false ); emit( "}break;" );
        emit( "case 10:{" ); compile( 10, false ); emit( "}break;" );
        emit( "case 11:{" ); compile( 11, false ); emit( "}break;" );
        emit( "default:{goto x;}};" );
        PRETTY( "mOO" );
        break;
    
    // Moo
    case 4:
        emit( "if((*p)!=0){putchar(*p);}else{(*p)=getchar();while(getchar()!='\\n');}" );
        PRETTY( "Moo" );
        break;
    
    // MOo
    case 5:
        emit( "(*p)--;" );
        PRETTY( "MOo" );
        break;
    
    // MoO
    case 6:
        emit( "(*p)++;" );
        PRETTY( "MoO" );
        break;

    // MOO
    case 7:
        {
            int level = 1;
            int num = moocount;
            int prev = 0;
            mem_t::iterator t = prog_pos;
            t++;	  // have to skip past next command when looking for next moo.

            if( t != program.end() )
            {
                if( (*t) == 0 )
                    num++;

                while( level > 0 )
                {
                    prev = *t;
                    t++;
                
                    if( t == program.end() )
                        break;
                
                    if( (*t) == 7 ) // look for MOO command.
                        level++;
                    else
                    if( (*t) == 0 )	// look for moo command.
                    {
                        if( prev == 7 )
                            level--;
                
                        level--;
                        num++;
                    }
                
                    if( level == 0 )
                        break;
                }
            }
            if( advance && level != 0 )
                quit();
            else if( level != 0 )
            {
                emit( "rterr();" );
                break;
            }
            
            MOOcount++;
            emit( "M%d:", MOOcount );
            emit( "if(!(*p))goto m%d;", num );
            PRETTY( "MOO" );
        }
        break;
    
    // OOO
    case 8:
        emit( "(*p)=0;" );
        PRETTY( "OOO" );
        break;

    // MMM
    case 9:
        emit( "if(h){(*p)=r;}else{r=(*p);}h=!h;" );
        PRETTY( "MMM" );
        break;

    // OOM
    case 10:
        emit( "printf(\"%%d\\n\",*p);" );
        PRETTY( "OOM" );
        break;
    
    // oom
    case 11:
        emit( "char b[100];int c=0;" );
        emit( "while(c<sizeof(b)-1){b[c]=getchar();c++;b[c]=0;if(b[c-1]=='\\n')break;}" );
        emit( "if(c==sizeof(b))while(getchar()!='\\n');(*p)=atoi(b);" );
        PRETTY( "oom" );
        break;

    // bad stuff
    default:
        return false;
    };

    if( advance )
        prog_pos++;

    return true;
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

	printf( "Compiling [%s]...\n", argv[1] );

    // init main memory.
    /*
    memory.push_back( 0 );
    mem_pos = memory.begin();
    */
    
    output.clear();
    output.reserve( 64 * 1024 );
    emit( "#include <stdio.h>\n" );
    emit( "#include <vector>\n" );
    emit( "typedef std::vector<int> t_;t_ m;t_::iterator p;\n" );
    emit( "bool h;int r;\n" );
    emit( "void rterr(){puts(\"Runtime error.\\n\");}\n" );
    emit( "int main(int a,char** v){\n" );
    emit( "m.push_back(0);p=m.begin();h=false;\n" );

    prog_pos = program.begin();
    while( prog_pos != program.end() )
        if( !compile( *prog_pos, true ) )
        {
            printf( "ERROR!\n" );
            break;
        }
        
    emit( "x:return(0);}\n" );

    FILE* out_file = fopen( "cow.out.cpp", "wb" );
    if( out_file == NULL )
    {
        printf( "Could not write output file.\n" );
        exit( 1 );
    }
    fwrite( output.data(), 1, output.size(), out_file );
    fclose( out_file );

    printf( "C++ source code: cow.out.cpp\n" );

    #ifdef COMPILER
        std::string path( (const char*)COMPILER );
        path.append( " " );
        path.append( (const char*)NAME_FLAG );
        path.append( " " );
        path.append( (const char*)OUTPUT_EXEC );
        path.append( " " );
        path.append( (const char*)FLAGS );
        path.append( " " );
        path.append( (const char*)OUTPUT_CPP );
        if( system( path.c_str() ) )
            printf( "\n\nCould not compile.  Possible causes:  C++ compiler is not installed, not in path, or not named '%s' or there is a bug in this compiler.\n\n", COMPILER );
        else
            printf( "Executable created: cow.out\n" );
    #endif

	return 0;
}

