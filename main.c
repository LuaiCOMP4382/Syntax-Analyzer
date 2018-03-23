#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TRUE 1
#define FALSE 0
#define TOKEN_COUNT_LIMIT 1000
#define TOKEN_LENGTH_LIMIT 100
#define RESERVED_WORD_COUNT 15

void error(char expectedToken[]);
int checkToken(char expectedToken[]);
void getNextToken();
const char * scanNextToken();
int isSpecialCharacter(int inputCharacter);

// Recurisve decent parsing methods
void module_declaration();
void module();
void module_heading();
void declarations();
void const_decl();
void const_item();
void var_decl();
void var_item();
void name_list();
void task_decl();
void task_heading();
void compound_stmt();
void stmt_list();
void statement();
void ass_stmt();
void arith_exp();
void term();
void factor();
void name_value();
void add_sign();
void mul_sign();
void inout_stmt();
void if_stmt();
void else_part();
void while_stmt();
void bool_exp();
void relational_oper();

FILE *sourceCode;
const char reservedWords[RESERVED_WORD_COUNT][8] = {"module", "const", "var", "integer", "task", "begin", "end", "input", "out", "if", "fi", "then", "else", "while", "do"};
char nextToken[TOKEN_LENGTH_LIMIT];
int line;
int wordIndex;
int caughtError;
int doneWithFile;

int main() {

    line = 1;
    wordIndex = 0;
    caughtError = FALSE;
    doneWithFile = FALSE;

    sourceCode = fopen("source_code.txt", "r");

    getNextToken();
    module_declaration();

    fclose(sourceCode);

    return 0;
}

void module_declaration() {

    module();

    if (caughtError)
        return;

    if (checkToken("."))
        printf("Correct syntax\n");
    else
        error(".");

}

void module() {

    module_heading();

    if (caughtError)
        return;

    declarations();

    if (caughtError)
        return;

    compound_stmt();

}


void module_heading() {

    if (checkToken("module")) {

        getNextToken();

        if (checkToken("name")) {

            getNextToken();

            if (checkToken(";"))
                getNextToken();
            else
                error(";");

        } else
            error("name");


    } else
        error("module");

}

void declarations() {

    const_decl();

    if (caughtError)
        return;

    var_decl();

    if (caughtError)
        return;

    task_decl();

}

void const_decl() {

    if (checkToken("const")) {

        getNextToken();

        if (checkToken("name")) {

            const_item();

            if (caughtError)
                return;

            if (checkToken(";")) {

                getNextToken();

                if (checkToken("name"))
                    while (checkToken("name")) {

                        const_item();

                        if (caughtError)
                            return;

                        if (checkToken(";"))
                            getNextToken();
                        else
                            error(";");

                    }

            } else
                error(";");

        } else
            error("name");

    } else if (!checkToken("var") && !checkToken("task") && !checkToken("begin"))
        error("const, var, task, begin");

}

void const_item() {

    if (checkToken("name")) {

        getNextToken();

        if (checkToken("=")) {

            getNextToken();

            if (checkToken("integer-value"))
                getNextToken();
            else
                error("integer-value");

        } else
            error("=");

    } else
        error("name");

}

void var_decl() {

    if (checkToken("var")) {

        getNextToken();

        if (checkToken("name")) {

            var_item();

            if (caughtError)
                return;

            if (checkToken(";")) {

                getNextToken();

                if (checkToken("name"))
                    while (checkToken("name")) {

                        var_item();

                        if (caughtError)
                            return;

                        if (checkToken(";"))
                            getNextToken();
                        else
                            error(";");

                    }

            } else
                error(";");

        } else
            error("name");

    } else if (!checkToken("task") && !checkToken("begin"))
        error("var, task, begin");

}

void var_item() {

    name_list();

    if (caughtError)
        return;

    if (checkToken(":")) {

        getNextToken();

        if (checkToken("integer"))
            getNextToken();
        else
            error("integer");

    } else
        error(":");

}

void name_list() {

    if (checkToken("name")) {

        getNextToken();

        if (checkToken(","))
            while (checkToken(",")) {

                getNextToken();

                if (checkToken("name"))
                    getNextToken();
                else
                    error("name");

            }

    } else
        error("name");

}

void task_decl() {

    if (checkToken("task")) {

        task_heading();

        if (caughtError)
            return;

        declarations();

        if (caughtError)
            return;

        compound_stmt();

        if (caughtError)
            return;

        if (checkToken(";"))
            getNextToken();
        else
            error(";");

    } else if (!checkToken("begin"))
        error("task, begin");

}

void task_heading() {

    if (checkToken("task")) {

        getNextToken();

        if (checkToken("name")) {

            getNextToken();

            if (checkToken(";"))
                getNextToken();
            else
                error(";");

        } else
            error("name");

    } else
        error("task");

}

void compound_stmt() {

    if (checkToken("begin")) {

        getNextToken();

        stmt_list();

        if (caughtError)
            return;

        if (checkToken("end"))
            getNextToken();
        else
            error("end");

    } else
        error("begin");

}

void stmt_list() {

    if (checkToken("name") || checkToken("input") || checkToken("out")
    || checkToken("if") || checkToken("while") || checkToken("begin")
    || checkToken(";")) {

        while (checkToken("name") || checkToken("input") || checkToken("out")
        || checkToken("if") || checkToken("while") || checkToken("begin")
        || checkToken(";")) {

            statement();

            if (caughtError)
                return;

            if (checkToken(";"))
                getNextToken();
            else
                error(";");

        }

    } else if (!checkToken("end"))
        error("name, input, out, if, while, begin, ;, end");

}

void statement() {

    if (checkToken("name"))
        ass_stmt();

    else if (checkToken("input") || checkToken("out"))
        inout_stmt();

    else if (checkToken("if"))
        if_stmt();

    else if (checkToken("while"))
        while_stmt();

    else if (checkToken("begin"))
        compound_stmt();

    else if (!checkToken("else") && !checkToken("fi") && !checkToken(";"))
        error("name, input, out, if, while, begin, else, fi, ;");

}

void ass_stmt() {

    if (checkToken("name")) {

        getNextToken();

        if (checkToken(":=")) {

            getNextToken();

            arith_exp();

        } else
            error(":=");

    } else
        error("name");

}

void arith_exp() {

    term();

    if (caughtError)
        return;

    if (checkToken("+") || checkToken("-"))
        while (checkToken("+") || checkToken("-")) {

            add_sign();

            if (caughtError)
                return;

            term();

            if (caughtError)
                return;

        }

}

void term() {

    factor();

    if (caughtError)
        return;

    if (checkToken("*") || checkToken("/"))
        while (checkToken("*") || checkToken("/")) {

            mul_sign();

            if (caughtError)
                return;

            factor();

            if (caughtError)
                return;

        }

}

void factor() {

    if (checkToken("(")) {

        getNextToken();

        arith_exp();

        if (caughtError)
            return;

        if (checkToken(")"))
            getNextToken();
        else
            error(")");

    } else if (checkToken("name") || checkToken("integer-value"))
        name_value();
    else
        error("(, name, integer-value");

}

void name_value() {

    if (checkToken("name"))
        getNextToken();
    else if (checkToken("integer-value"))
        getNextToken();
    else
        error("name, integer-value");

}

void add_sign() {

    if (checkToken("+"))
        getNextToken();
    else if (checkToken("-"))
        getNextToken();
    else
        error("+, -");

}

void mul_sign() {

    if (checkToken("*"))
        getNextToken();
    else if (checkToken("/"))
        getNextToken();
    else
        error("-, /");

}

void inout_stmt() {

    if (checkToken("input")) {

        getNextToken();

        if (checkToken("(")) {

            getNextToken();

            if (checkToken("name")) {

                getNextToken();

                if (checkToken(")"))
                    getNextToken();
                else
                    error(")");

            } else
                error("name");

        } else
            error("(");

    } else if (checkToken("out")) {

        getNextToken();

        if (checkToken("(")) {

            getNextToken();

            name_value();

            if (caughtError)
                return;

            if (checkToken(")"))
                    getNextToken();
                else
                    error(")");

        } else
            error("(");

    } else
        error("input, out");

}

void if_stmt() {

    if (checkToken("if")) {

        getNextToken();

        bool_exp();

        if (caughtError)
            return;

        if (checkToken("then")) {

            getNextToken();

            statement();

            if (caughtError)
                return;

            else_part();

            if (caughtError)
                return;

            if (checkToken("fi"))
                getNextToken();
            else
                error("fi");

        } else
            error("then");

    } else
        error("if");

}

void else_part() {

    if (checkToken("else")) {

        getNextToken();

        statement();

    } else if (!checkToken("fi"))
        error("else, fi");

}

void while_stmt() {

    if (checkToken("while")) {

        getNextToken();

        bool_exp();

        if (caughtError)
            return;

        if (checkToken("do")) {

            getNextToken();

            statement();

        } else
            error("do");

    } else
        error("while");

}

void bool_exp() {

    name_value();

    if (caughtError)
        return;

    relational_oper();

    if (caughtError)
        return;

    name_value();

}

void relational_oper() {

    if (checkToken("="))
        getNextToken();
    else if (checkToken("<>"))
        getNextToken();
    else if (checkToken("<"))
        getNextToken();
    else if (checkToken("<="))
        getNextToken();
    else if (checkToken(">"))
        getNextToken();
    else if (checkToken(">="))
        getNextToken();
    else
        error("=, <>, <=, >, >=");

}

// UTILITIES

void error(char expectedToken[]) {

    caughtError = TRUE;
    printf("Error at line %d, token number %d\nToken found: %s\nExpected: %s", line, wordIndex, nextToken, expectedToken);

}

int checkToken(char expectedToken[]) {

    if (strcmp(nextToken, expectedToken) == 0)
        return TRUE;
    else
        return FALSE;

}

void getNextToken() {

    if (doneWithFile) {

        strcpy(nextToken, "No more tokens");
        return;

    }

    strcpy(nextToken, scanNextToken());

    if (checkToken("No more tokens"))
        doneWithFile = TRUE;

}

const char * scanNextToken() {

    int nextChar;

    while((nextChar = fgetc(sourceCode)) != EOF) {

        if (nextChar == '\n') {

            line++;
            wordIndex = 0;

        } else if (isalpha(nextChar)) {

            int currentEndingIndex = 1;
            static char currentToken[TOKEN_LENGTH_LIMIT];

            currentToken[0] = (char) nextChar;
            currentToken[1] = '\0';

            nextChar = fgetc(sourceCode);

            while (isalpha(nextChar) || isdigit(nextChar)) {

                currentToken[currentEndingIndex] = (char) nextChar;
                currentEndingIndex++;
                currentToken[currentEndingIndex] = '\0';

                nextChar = fgetc(sourceCode);

            }

            ungetc((char) nextChar, sourceCode);

            int i;
            for (i = 0; i < RESERVED_WORD_COUNT; i++)
                if (strcmp(reservedWords[i], currentToken) == 0)
                    return currentToken;

            wordIndex++;
            return "name";

        } else if (isdigit(nextChar)) {

            while (isdigit(nextChar))
                nextChar = fgetc(sourceCode);

            ungetc((char) nextChar, sourceCode);

            wordIndex++;
            return "integer-value";

        } else if (isSpecialCharacter(nextChar)) {

            if ((char) nextChar == '<') {

                nextChar = fgetc(sourceCode);

                wordIndex++;
                if ((char) nextChar == '>')
                    return "<>";
                else if ((char) nextChar == '=')
                    return "<=";

                ungetc((char) nextChar, sourceCode);

                return "<";

            } else if ((char) nextChar == '>') {

                nextChar = fgetc(sourceCode);

                wordIndex++;
                if ((char) nextChar == '=')
                    return ">=";

                ungetc((char) nextChar, sourceCode);

                return ">";

            } else if ((char) nextChar == ':') {

                nextChar = fgetc(sourceCode);

                wordIndex++;
                if ((char) nextChar == '=')
                    return ":=";

                ungetc((char) nextChar, sourceCode);

                return ":";

            } else {

                wordIndex++;
                static char ret[2] = {'0', '\0'};
                ret[0] = (char) nextChar;

                return ret;

            }

        }

    }

    return "No more tokens";

}

int isSpecialCharacter(int inputCharacter) {

    char character = (char) inputCharacter;

    if
    (
       character == '.' || character == ';' || character == '=' || character == ':' || character == ','
    || character == '(' || character == ')' || character == '+' || character == '-' || character == '*'
    || character == '/' || character == '<' || character == '>'
    )
        return TRUE;

    return FALSE;

}
