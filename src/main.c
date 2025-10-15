#include <stdio.h>
#include <stdlib.h>
#include "jms_oop_utils/jms_static_ctors_dtors.h"
#include "jms_parse/jms_parser.h"
#include "jms_token.h"
#include "jms_utils/jms_vector.h"
#include "jms_tests/jms_unitTests.h"
#include "jms_lex.h"
#include "jms_utils/jms_str.h"
#include "jms_parse/jms_token.h"

#define JMS_UNIT_TESTS_ON

int run_program(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    jms_static_ctors_run();
    int result = run_program(argc, argv);
    jms_static_dtors_run();

    return result;
}

int run_program(int argc, char* argv[])
{
    printf("main: before jms_unitTests_run\n");
#ifdef JMS_UNIT_TESTS_ON
    jms_unitTests_run();
#endif
    fflush(stdout);
    printf("main: after jms_unitTests_run\n");

    if (argc < 1)
    {
        fprintf(stderr, "no file specified\n");
        return 1;
    }

    printf("filePath == %s\n", argv[1]);
    char*       filePath = argv[1];

    printf("main: before lexFile\n");

    JMS_OWNED_PTR(jms_lexer)  lexer
        = jms_lex_init();
    JMS_OWNED_PTR(jms_vector) lexedTokens
        = jms_lexFile(lexer, filePath);
        
    printf("main: after lexFile\n");

    //printf("main.c/main: elemCount(lexedTokens) == %zu",
    //    jms_vec_elemCount(lexedTokens));

    if (lexedTokens != NULL)
    {
        printf("main: before print loop\n");
        for (size_t i = 0; i < jms_vec_elemCount(lexedTokens); i++)
        {
            JMS_BORROWED_PTR(jms_token)
                elem = jms_vec_get(lexedTokens, i);

            // This will look much nicer in the self-hosted compiler:
            //
            //      var cStr = elem.getText().cStr();
            JMS_BORROWED_PTR(char)
                cStr = jms_str_cStr(jms_tok_getText(elem));
                
            printf("main: token %zu, at (line %d, col %d): %s\n",
                        i, jms_tok_getLineNum(elem), jms_tok_getColNum(elem) , cStr);
        }
    }
    else
    {
        fprintf(stderr, "[%s]: Error lexing tokens\n",
            __func__);
    }
    printf("main: after print loop\n");
    
    fflush(stdout);
    fflush(stderr);

    JMS_OWNED_PTR(jms_parser)
        parser = jms_parser_init(lexedTokens);

    JMS_OWNED_PTR(jms_vector)
        ast = NULL;

    if (parser != NULL)
    {
        ast = jms_parser_parse(parser);
        jms_parser_del(parser);
    }
    else
    {
        fprintf(stderr, "[%s]: Error creating parser\n",
            __func__);
    }

    printf("main: before ast print loop\n");
    if (ast != NULL)
    {
        printf("main: ast size == %d\n",
            jms_vec_elemCount(ast));
        for (size_t i = 0; i < jms_vec_elemCount(ast); i++)
        {
            JMS_BORROWED_PTR(jms_token)
                elem = jms_vec_get(ast, i);

            // This will look much nicer in the self-hosted compiler:
            //
            //      var cStr = elem.getText().cStr();
            JMS_BORROWED_PTR(char)
                cStr = jms_str_cStr(jms_token_getText(elem));
                
            printf("main: ast node %zu: %s\n",
                        i, cStr);
        }
    }
    else
    {
        fprintf(stderr, "[%s]: Error creating AST\n",
            __func__);
    }
    printf("main: after ast print loop\n");

    jms_vec_del(ast);
    jms_vec_del(lexedTokens);
    jms_lex_del(lexer);

    fflush(stdout);
    fflush(stderr);
    
    printf("[%s]: TESTING THAT VS CODE UPDATED FILE #2\n", __FUNCTION__);

    return 0;
}