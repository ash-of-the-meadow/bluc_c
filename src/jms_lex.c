#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "comments_remover.h"
#include "jms_lex.h"
#include "jms_token.h"
#include "jms_utils/jms_freader.h"
#include "jms_utils/jms_vector.h"
#include "jms_utils/jms_str.h"
#include "jms_utils/jms_ptr_annotations.h"
#include "jms_utils/jms_stdint.h"
#include "jms_utils/jms_strUtil.h"

typedef struct Lexer {
    JMS_OWNED_PTR(jms_str) fileContents;
    char curChar;
    int curCharIndex;

    int line;
} Lexer;

jms_lexer* jms_lex_init()
{
    jms_lexer* self = malloc(sizeof(jms_lexer));

    self->curCharIndex  = 0;
    self->line          = 1;
    self->fileContents  = jms_str_init("");

    return self;
}

void jms_lex_del(jms_lexer* self)
{
    free(self->fileContents);
    free(self);
}

/**
 * Global function - no class instance needed
 */
static void jms_lex_appendIfNotWhitespace(
    JMS_BORROWED_PTR(jms_vector) lexedTokens,
    JMS_BORROWED_PTR(jms_str) filePath,
    int32_t lineNum,
    int32_t colNum,
    JMS_BORROWED_PTR(jms_str) wordSoFar)
{
    if (!jms_str_isEmpty(wordSoFar) && !jms_str_isWhitespace(wordSoFar))
    {
        jms_token* token = jms_tok_init(
            jms_str_init_str(filePath),
            lineNum,
            colNum,
            jms_str_init_str(wordSoFar));

        // Function pointers are why we can't have nice things in c. That, and lack of real generics.
        jms_vec_add(lexedTokens, token, ((void (*) (void*))jms_tok_del));

        // Reset the "Word so far" to an empty string.
        jms_str_set_cStr(wordSoFar, "");
    }
}

/// @brief Returns whether the given char is equal to the given cString.
/// @param searchCriteria - The char to compare.
/// @param actualElement - The cString to compare against.
/// @return Whether the char is equal to the first character of the cString, or not.
static bool jms_lex_comparer_charCStr(void* searchCriteria, void* actualElement)
{
    JMS_BORROWED_PTR(char) searchChar    = (char*) searchCriteria;
    JMS_BORROWED_PTR(char) elementAsCStr = (char*) actualElement;
    
    char temp[2] = { *searchChar, '\0' };
    return jms_strUtil_cstrEq(elementAsCStr, temp);
}

/// @brief Compares the given jms_str, which should only be a length of 1,
//      to the first character in the given cString.
/// @param searchCriteria - The jms_str to compare. Should only be a length of 1.
/// @param actualElement - The cString to compare against.
/// @return Whether the first character of the cString is equal to the value in jms_str, or not.
static bool jms_lex_comparer_stringChar(void* searchCriteria, void* actualElement)
{
    JMS_BORROWED_PTR(jms_str)
        searchStr = (jms_str*) searchCriteria;
    JMS_BORROWED_PTR(char)
        elementAsCStr = (char*) actualElement;
        
    return jms_str_eq_cStr(searchStr, elementAsCStr);
}

// TODO - 4/23/2025 - refactor this some day so that it's not as long and confusing.
static JMS_XFER_PTR(jms_vector) jms_lex(
        JMS_BORROWED_PTR(jms_lexer) self,
        JMS_BORROWED_PTR(jms_str) filePath)
{
    JMS_OWNED_PTR(jms_str) fileTextWithoutComments
        = jms_cremover_run(self->fileContents);

    // printf("[%s]: fileTextWithoutComments == `%s`\n", __FUNCTION__, jms_str_cStr(fileTextWithoutComments));

    i32 lineNum = 1;
    i32 column = 1;
    
    bool
        wasLastCharEscaped = false;
    bool
        isInString = false;

    JMS_XFER_PTR(jms_vector)
        lexedTokens = jms_vec_init(sizeof(jms_str*));

    JMS_OWNED_PTR(jms_vector)
        singleTokenLexemesVec = jms_vec_init(sizeof(char*));
    JMS_OWNED_PTR(jms_vector)
        multiTokenLexemesVec = jms_vec_init(sizeof(char*));

    // "Special character" lexemes which will *definitely* be a single character.
    jms_vec_addAll(singleTokenLexemesVec, 8, NULL,
                        "(", ")", "{", "}", "[", "]", ";", ".");

    // "Special character" lexemes which *may* be multiple characters.
    jms_vec_addAll(multiTokenLexemesVec, 23, NULL,
                        "=", "==", "!=", "+=", "-=", "*=", "/=", "%=", "<=", ">=",
                        "&&", "||", "++", "--", "\\", "\\\\", "\r", "\n", "\t", "!",
                        ",,", ",", "<", ">");

    JMS_OWNED_PTR(jms_str)
        wordSoFar = jms_str_init("");

    // char value indicating that we're at the start of a file
    const char      NO_PREVIOUS_CHAR    = '\0';

    // char value indicating that we're at the end of a file
    const char      NO_NEXT_CHAR        = '\0';
    const size_t    indexOfLastToken    = jms_str_len(fileTextWithoutComments) - 1;

    // The number of characters to skip lexing.
    //  This is used by multi-token lexemes to
    //  skip over chars they've already processed.
    size_t          charsToSkip        = 0;

    // Loop through the file text, character by character.
    for (size_t i = 0; i <= indexOfLastToken; i++)
    {
        if (charsToSkip > 0 && i < indexOfLastToken)
        {
            charsToSkip--;
            continue;
        }

        // current character
        char curChar = jms_str_charAt(fileTextWithoutComments, i);

        // previous character
        char prevChar = (i > 0)
                            ? jms_str_charAt(fileTextWithoutComments, i - 1)
                            : NO_PREVIOUS_CHAR;

        char nextChar = (i < indexOfLastToken)
                            ? jms_str_charAt(fileTextWithoutComments, i + 1)
                            : NO_PREVIOUS_CHAR;
                            
        if (isInString)
        {
            if (curChar == '\"')
            {
                if (wasLastCharEscaped)
                {
                    // We have an escaped quote, so we need to add it to the word.
                    jms_str_append_ch(wordSoFar, curChar);

                    printf("[%s]: got here #1, word ==`%s`\n", __FUNCTION__, jms_str_cStr(wordSoFar));
                    wasLastCharEscaped = false;
                }
                else
                {
                    // We have the end of a string literal, so we need to add it to the word.
                    printf("[%s]: got here #2, word == `%s`\n", __FUNCTION__, jms_str_cStr(wordSoFar));

                    jms_lex_appendIfNotWhitespace(
                        lexedTokens,
                        filePath,
                        lineNum,
                        column,
                        wordSoFar);
                    
                    wasLastCharEscaped = false;
                    isInString = false;
                }

                wasLastCharEscaped = false;
            }

            jms_str_append_ch(wordSoFar, curChar);
        }
        else if (curChar == '\"')
        {
            if (wasLastCharEscaped)
            {
                // We have an escaped quote, so we need to add it to the word.
                jms_str_append_ch(wordSoFar, curChar);
            }
            else
            {
                // We have the start of a string literal, so we need to add it to the word.
                jms_lex_appendIfNotWhitespace(
                    lexedTokens,
                    filePath,
                    lineNum,
                    column,
                    wordSoFar);

                jms_str_append_ch(wordSoFar, curChar);

                jms_lex_appendIfNotWhitespace(
                    lexedTokens,
                    filePath,
                    lineNum,
                    column,
                    wordSoFar);

                isInString = true;
            }

            wasLastCharEscaped = false;
        }
        else if (curChar == '\r')
        {
            // Ignore carriage returns.
            // This is a special case for Windows-style line endings.
            //
            // This is only skipping *actual* carriage returns, not
            //  carriage returns that are embedded in string literals.
            continue;
        }
        else if (isspace(curChar))
        {
            jms_lex_appendIfNotWhitespace(
                lexedTokens,
                filePath,
                lineNum,
                column - 1, // We're adding the *previous* token.
                wordSoFar);
        }
        // else if the char is in the "single token lexemes" vector
        else if (jms_vec_find(singleTokenLexemesVec, &curChar, jms_lex_comparer_charCStr))
        {
            jms_lex_appendIfNotWhitespace(
                lexedTokens,
                filePath,
                lineNum,
                column,
                wordSoFar);

            // wordSoFar should be an empty string at this point.
            jms_str_append_ch(wordSoFar, curChar);

            printf("[%s]: found a single token lexeme, word == `%s`, line == %d, col == %d\n",
                     __FUNCTION__, jms_str_cStr(wordSoFar), lineNum, column);

            jms_lex_appendIfNotWhitespace(
                lexedTokens,
                filePath,
                lineNum,
                column,
                wordSoFar);
        }
        else
        {
            JMS_OWNED_PTR(jms_str) curCharAndNextChar
                = jms_str_init_ch(curChar);

            jms_str_append_ch(curCharAndNextChar, nextChar);

            printf("[%s]: curChar == `%c`, nextChar == `%c`, curCharAndNextChar == `%s`\n",
                     __FUNCTION__, curChar, nextChar, jms_str_cStr(curCharAndNextChar));

            // Check if a token type consisting of the current character and the next character
            //  are in the multi-token lexemes vector.
            if (jms_vec_find(multiTokenLexemesVec, curCharAndNextChar, jms_lex_comparer_stringChar))
            {
                // Add the word that was detected before this multi-token lexeme.
                jms_lex_appendIfNotWhitespace(
                    lexedTokens,
                    filePath,
                    lineNum,
                    column,
                    wordSoFar);

                // We have a multi-token lexeme, so we need to add it to the word.
                jms_str_set_cStr_s(wordSoFar, curCharAndNextChar);

                printf("[%s]: found a multi token lexeme, word == `%s`, line == %d, col == %d\n",
                        __FUNCTION__, jms_str_cStr(wordSoFar), lineNum, column);

                jms_lex_appendIfNotWhitespace(
                    lexedTokens,
                    filePath,
                    lineNum,
                    column,
                    wordSoFar);

                // We need to skip the next character since we've already processed it.
                charsToSkip = 1;
            }
            // else if the previous token was NOT in the "multi token lexemes" vector
            //      [so we don't incorrectly match a "single" token too early]
            //  AND the current token IS in the "multi token lexemes" vector..
            else if (
                !jms_vec_find(multiTokenLexemesVec, &prevChar, jms_lex_comparer_charCStr)
                && jms_vec_find(multiTokenLexemesVec, &curChar, jms_lex_comparer_charCStr))
            {
                // We have a single token lexeme which is stored in the "multi token lexemes" vector,
                //  so we need to add it to the tokens list
                jms_str_append_ch(wordSoFar, curChar);

                printf("[%s]: found a single-tok lexeme in multi-tok vector, word == `%s`, line == %d, col == %d\n",
                     __FUNCTION__, jms_str_cStr(wordSoFar), lineNum, column);

                jms_lex_appendIfNotWhitespace(
                    lexedTokens,
                    filePath,
                    lineNum,
                    column,
                    wordSoFar);
            }
            // We're just on a regular character, so we need to add it to the word.
            else
            {
                jms_str_append_ch(wordSoFar, curChar);
            }

            jms_str_del(curCharAndNextChar);
        }

        if (curChar == '\n')
        {
            lineNum++;
            column = 1;
        }
        else
        {
            column++;
        }
    }

    jms_vec_del(singleTokenLexemesVec);
    jms_vec_del(multiTokenLexemesVec);
    jms_str_del(wordSoFar);
    jms_str_del(fileTextWithoutComments);

    return lexedTokens;
}

JMS_XFER_PTR(jms_vector) jms_lexFile(jms_lexer* self, const char* filePath)
{
    JMS_XFER_PTR(jms_vector)
        lexedTokens;
    JMS_OWNED_PTR(jms_freader)
        reader = jms_freader_init(filePath);

    self->fileContents = jms_freader_readLines(reader);

    if (self->fileContents == NULL)
    {
        fprintf(stderr, "[%s]: Error reading input file \"%s\". Please check that "
            "the path and file exist.\n", __FUNCTION__, filePath);
        lexedTokens = NULL;
    }
    else
    {
        JMS_XFER_PTR(jms_str)
            filePathAsStr = jms_str_init(filePath);

        lexedTokens = jms_lex(self, filePathAsStr);

        jms_str_del(filePathAsStr);
    }

    jms_freader_del(reader);
    
    return lexedTokens;
}