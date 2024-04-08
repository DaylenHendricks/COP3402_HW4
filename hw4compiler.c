//Homework 4: PL/0 Compiler
//Authors: Daylen Hendricks, Brandon Ramlagan
//COP 3402 Spring 2024
//Date 4/07/2024



//libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//static values
#define MAXNUM 5 //max number of digits for a number
#define MAXIDENTIFIER 11 //max length of identifier name
#define MAX_SYMBOL_TABLE_SIZE 500 //max size for symbol table

typedef struct symbol
{
    int kind; // const = 1, var = 2, proc = 3
    char name[11]; // name up to 11 chars
    int val; // number (ASCII value)
    int level; // L level
    int addr; // M address
    int mark; // to indicate unavailable or deleted
}symbol;

typedef struct code
{
    int opcode;
    int l;
    int m;
}code;

typedef enum {
oddsym = 1, identsym, numbersym, plussym, minussym,
multsym, slashsym, fisym, eqsym, neqsym, lessym, leqsym,
gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
periodsym, becomessym, beginsym, endsym, ifsym, thensym,
whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
readsym} token_type;

//global variables
int tokenArr[500] = {0};//finalized token array
int tokenIndex = 0;
symbol symbolTable[MAX_SYMBOL_TABLE_SIZE];
code codeTable[MAX_SYMBOL_TABLE_SIZE];
int cx = 0;//codeTable index
int tp = 1; //symbol table pointer
int varCount = 0;
int token = 0;//token for parser
int finalToken;//token for period check
char assemblyTable[50][3];//table for assembly code
int assemIndex = 0;
int lexlvl = -1;
int firstBlock = 0;

void printAssembly(FILE *fptr);
void block(char identArray[50][12]);
void insertSymbolTable(int kind, char name[11], int val, int level, int addr); //insert into symbol table
int symbolTableCheck(char name[11]);
void ConstDeclaration(char identArr[50][12]);
int VarDeclaration(char identArray[50][12]); //returns number of variables
void ProcedureDeclaration(char identArray[50][12]);
int STATEMENT(char identArray[50][12]);
void CONDITION(char identArray[50][12]);
void EXPRESSION(char identArray[50][12]);
void TERM(char identArray[50][12]);
void FACTOR(char identArray[50][12]);


int main(int argc, char *fileName[])
{
    int fileArr[500] = {0};//array to store input
    int index = 0;//array index
    int curVal = 0;//current input value
    int flag = 0;//EOF flag
    int lastVal;
    char identArr[50][12];
    for(int i = 0; i <=50 ; i++)//"#" used for HW3 checking end of strings
    {
        for(int j = 0; j <= 50; j++)
        {
        identArr[i][j] = '#';
        }
    }
    FILE* inputFile = fopen(fileName[1], "r");//file name string for input

//printing source code

        fseek(inputFile, 0, SEEK_SET);//set file pointer to beginning again

        while(flag == 0)//loop through text and insert in array
        {
            curVal = (int)fgetc(inputFile);//getting next value & cast to int
            if(curVal == 47)//finding "/"
            {
                lastVal = curVal;
                curVal = (int)fgetc(inputFile);//getting next value & cast to int
                if(curVal == 42 && lastVal == 47)//finding "/*"
                {
                    int commentFlag = 0;//flag for comment exclusion loop
                    while(commentFlag == 0)
                    {
                        lastVal = curVal;
                        curVal = (int)fgetc(inputFile);//getting next value & cast to int
                        if(curVal == 47 && lastVal == 42)
                        {
                            break;//want to break out of comment loop
                        }
                    }
                }
                else//if just "/"
                {
                fileArr[index] = lastVal;//store "/"
                index++;//move on
                }
            }
            else if(curVal == -1)//end of file detection
            {
                flag = 1;
            }
            else if((curVal >= 0 && curVal <= 32))// \n to whitespace
            {
                fileArr[index] = 32;//insert value into array
                index++;//incrementing to next array index
            }
            else
            {
                fileArr[index] = curVal;//insert value into array
                index++;//incrementing to next array index
            }
        }
        // print out lexeme headers
        // printf("\nLexeme Table:\n\n");
        // printf("lexeme token type\n");


//begin tokenization
        int i = 0; 
        int flag1 = 0;    
        while(flag1 == 0)
        {

//reserved words        
            if(fileArr[i] == 32)//space
            {
                i++;
            }
            //const
            else if(fileArr[i] == 99 && fileArr[i+1] == 111 && fileArr[i+2] == 110 && fileArr[i+3] == 115 && fileArr[i+4] == 116 && (fileArr[i+5] < 65 || fileArr[i+5] > 90) && (fileArr[i+5] < 97 || fileArr[i+5] > 122))
            {
                tokenArr[tokenIndex] = constsym;
                tokenIndex++;
                i += 5;
                // printf("const\t28\n");
            }
            
            //var
            else if(fileArr[i] == 118 && fileArr[i+1] == 97 && fileArr[i+2] == 114 && (fileArr[i+3] < 65 || fileArr[i+3] > 90) && (fileArr[i+3] < 97 || fileArr[i+3] > 122))
            {
                tokenArr[tokenIndex] = varsym;
                tokenIndex++;
                i += 3;
                // printf("var\t29\n");
            }

            //procedure
            else if(fileArr[i] == 112 && fileArr[i+1] == 114 && fileArr[i+2] == 111 && fileArr[i+3] == 99 && fileArr[i+4] == 101 && fileArr[i+5] == 100 && fileArr[i+6] == 117 && fileArr[i+7] == 114 && fileArr[i+8] == 101 && (fileArr[i+9] < 65 || fileArr[i+9] > 90) && (fileArr[i+9] < 97 || fileArr[i+9] > 122))
            {
                tokenArr[tokenIndex] = procsym;
                tokenIndex++;
                i += 9;
                // printf("procedure 30\n");
            }

            //call
            else if(fileArr[i] == 99 && fileArr[i+1] == 97 && fileArr[i+2] == 108 && fileArr[i+3] == 108 && (fileArr[i+4] < 65 || fileArr[i+4] > 90) && (fileArr[i+4] < 97 || fileArr[i+4] > 122))
            {
                tokenArr[tokenIndex] = callsym;
                tokenIndex++;
                i += 4;
                // printf("call\t27\n");
            }

            //begin
            else if(fileArr[i] == 98 && fileArr[i+1] == 101 && fileArr[i+2] == 103 && fileArr[i+3] == 105 && fileArr[i+4] == 110 && (fileArr[i+5] < 65 || fileArr[i+5] > 90) && (fileArr[i+5] < 97 || fileArr[i+5] > 122))
            {
                tokenArr[tokenIndex] = beginsym;
                tokenIndex++;
                i += 5;
                // printf("begin\t21\n");
            }

            //end
            else if(fileArr[i] == 101 && fileArr[i+1] == 110 && fileArr[i+2] == 100 && (fileArr[i+3] < 65 || fileArr[i+3] > 90) && (fileArr[i+3] < 97 || fileArr[i+3] > 122))
            {
                tokenArr[tokenIndex] = endsym;
                tokenIndex++;
                i += 3;
                // printf("end\t22\n");
            }
            //if
            else if(fileArr[i] == 105 && fileArr[i+1] == 102 && (fileArr[i+2] < 65 || fileArr[i+2] > 90) && (fileArr[i+2] < 97 || fileArr[i+2] > 122))
            {
                tokenArr[tokenIndex] = ifsym;
                tokenIndex++;
                i += 2;
                // printf("if\t23\n");
            }
            //fi?
            else if(fileArr[i] == 102 && fileArr[i+1] == 105 && (fileArr[i+2] < 65 || fileArr[i+2] > 90) && (fileArr[i+2] < 97 || fileArr[i+2] > 122))
            {
                tokenArr[tokenIndex] = fisym;
                tokenIndex++;
                i += 2;
                // printf("fi\t8\n");
            }
            //then
            else if(fileArr[i] == 116 && fileArr[i+1] == 104 && fileArr[i+2] == 101 && fileArr[i+3] == 110 && (fileArr[i+4] < 65 || fileArr[i+4] > 90) && (fileArr[i+4] < 97 || fileArr[i+4] > 122))
            {
                tokenArr[tokenIndex] = thensym;
                tokenIndex++;
                i += 4;
                // printf("then\t24\n");
            }
            //while
            else if(fileArr[i] == 119 && fileArr[i+1] == 104 && fileArr[i+2] == 105 && fileArr[i+3] == 108 && fileArr[i+4] == 101 && (fileArr[i+5] < 65 || fileArr[i+5] > 90) && (fileArr[i+5] < 97 || fileArr[i+5] > 122))
            {
                tokenArr[tokenIndex] = whilesym;
                tokenIndex++;
                i += 5;
                // printf("while\t25\n");
            }
            //do
            else if(fileArr[i] == 100 && fileArr[i+1] == 111 && (fileArr[i+2] < 65 || fileArr[i+2] > 90) && (fileArr[i+2] < 97 || fileArr[i+2] > 122))
            {
                tokenArr[tokenIndex] = dosym;
                tokenIndex++;
                i += 2;
                // printf("do\t26\n");
            }
            //read
            else if(fileArr[i] == 114 && fileArr[i+1] == 101 && fileArr[i+2] == 97 && fileArr[i+3] == 100 && (fileArr[i+4] < 65 || fileArr[i+4] > 90) && (fileArr[i+4] < 97 || fileArr[i+4] > 122))
            {
                tokenArr[tokenIndex] = readsym;
                tokenIndex++;
                i += 4;
                // printf("read\t32\n");
            }
            //write
            else if(fileArr[i] == 119 && fileArr[i+1] == 114 && fileArr[i+2] == 105 && fileArr[i+3] == 116 && fileArr[i+4] == 101 && (fileArr[i+5] < 65 || fileArr[i+5] > 90) && (fileArr[i+5] < 97 || fileArr[i+5] > 122))
            {
                tokenArr[tokenIndex] = writesym;
                tokenIndex++;
                i += 5;
                // printf("write\t31\n");
            }
            else if(fileArr[i] == 111 && fileArr[i+1] == 100 && fileArr[i+2] == 100)
            {
                tokenArr[tokenIndex] = oddsym;
                tokenIndex++;
                i += 3;
                // printf("odd\t31\n");
            }
            


    //identifiers
            else if((fileArr[i] >= 65 && fileArr[i] <= 90) || (fileArr[i] >= 97 && fileArr[i] <= 122))
            {
                int flag2 = 0;
                int identLen = 0;
                tokenArr[tokenIndex] = identsym;
                tokenIndex++;
                while(flag2 == 0)
                {
                    if((fileArr[i] >= 65 && fileArr[i] <= 90) || (fileArr[i] >= 97 && fileArr[i] <= 122) || (fileArr[i] >= 48 && fileArr[i] <= 57))
                    {
                                identArr[varCount][identLen] = fileArr[i];
                                //printf("%c", fileArr[i]);
                                i++;
                                identLen++;
                    }
                     else
                    {
                        flag2 = 1;
                        if(identLen > 11)
                        {
                            printf("\tError: Name too long\n");
                            exit(0);
                        }
                        else
                        {
                            // printf("\t2\n");

                        }
                    }
                }
                tokenArr[tokenIndex] = identLen;
                tokenIndex++;
                varCount++;



            }
//numbers
            else if(fileArr[i] >= 48 && fileArr[i] <= 57)
            {
                char num[20];
                tokenArr[tokenIndex] = numbersym;
                tokenIndex++;
                int numLen = 0;
                while(fileArr[i] >= 48 && fileArr[i] <= 57)
                {
                    num[numLen] = fileArr[i];
                    numLen++;
                    i++;
                }
                int c = atoi(num);
                //printf("%d", c);
                tokenArr[tokenIndex] = c;
                tokenIndex++;
                if(numLen >5)
                {
                    printf("Error: Number too long\n");
                    exit(0);
                }
                else
                {
                    // printf("\t3\n");
                }

            }
//symbols
            //lparentsym
           else if(fileArr[i] == 40)
            {
                tokenArr[tokenIndex] = lparentsym;
                tokenIndex++;
                i++;
                // printf("(\t15\n");
            }
            //rparentsym
           else if(fileArr[i] == 41)
            {
                tokenArr[tokenIndex] = rparentsym;
                tokenIndex++;
                i++;
                // printf(")\t16\n");
            }
            //mult
           else if(fileArr[i] == 42)
            {
                tokenArr[tokenIndex] = multsym;
                tokenIndex++;
                i++;
                // printf("*\t6\n");
            }
            //plus
           else if(fileArr[i] == 43)
            {
                tokenArr[tokenIndex] = plussym;
                tokenIndex++;
                i++;
                // printf("+\t4\n");
            }
            //comma
           else if(fileArr[i] == 44)
            {
                tokenArr[tokenIndex] = commasym;
                tokenIndex++;
                i++;
                // printf(",\t17\n");
            }
            //minus
           else if(fileArr[i] == 45)
            {
                tokenArr[tokenIndex] = minussym;
                tokenIndex++;
                i++;
                // printf("-\t5\n");
            }
            //period
            else if(fileArr[i] == 46)
            {
                tokenArr[tokenIndex] = periodsym;
                tokenIndex++;
                i++;
                flag1 = 1;
                // printf(".\t19\n\n\n");
            }
            //slash
           else if(fileArr[i] == 47)
            {
                tokenArr[tokenIndex] = slashsym;
                tokenIndex++;
                i++;
                // printf("/\t7\n");
            }
            //become
            else if(fileArr[i] == 58 && fileArr[i+1] == 61)
            {
                tokenArr[tokenIndex] = becomessym;
                tokenIndex++;
                i += 2;
                // printf(":=\t20\n");
            }
            //semicolon
            else if(fileArr[i] == 59)
            {
                tokenArr[tokenIndex] = semicolonsym;
                tokenIndex++;
                i++;
                // printf(";\t18\n");
            }
            //equal
            else if(fileArr[i] == 61)
            {
                tokenArr[tokenIndex] = eqsym;
                tokenIndex++;
                i++;
                // printf("=\t9\n");
            }
            //not equal
            else if(fileArr[i] == 60 && fileArr[i+1] == 62)
            {
                tokenArr[tokenIndex] = neqsym;
                tokenIndex++;
                i += 2;
                // printf("<>\t10\n");
            }
          //less than or equal
            else if(fileArr[i] == 60 && fileArr[i+1] == 61)
            {
                tokenArr[tokenIndex] = leqsym;
                tokenIndex++;
                i += 2;
                // printf("<=\t12\n");
            }
          //less than
            else if(fileArr[i] == 60)
            {
                tokenArr[tokenIndex] = lessym;
                tokenIndex++;
                i++;
                // printf("<\t11\n");
            }
            //greater or equal
            else if(fileArr[i] == 62 && fileArr[i+1] == 61)
            {
                tokenArr[tokenIndex] = geqsym;
                tokenIndex++;
                i+=2;
                // printf(">=\t14\n");
            }
            //greater than
            else if(fileArr[i] == 62)
            {
                tokenArr[tokenIndex] = gtrsym;
                tokenIndex++;
                i++;
                // printf(">\t13\n");
            }
            else if (fileArr[i]== 0)//null terminator for when end of file is string
            {
                flag1 = 1;
            }
            else
            {
                printf("%c\tInvalid Symbol\n", fileArr[i]);
                exit(0);
                i++;
            }
        }//tokenize while loop end

        //printing token list
        // printf("Token List:\n");
        varCount = 0;
        // for(int i = 0; i < tokenIndex; i++)
        // {
        //     if(tokenArr[i] == identsym)
        //     {
        //         printf("%d ", tokenArr[i]);
        //         i++;
        //         for(int j = 0; j < tokenArr[i]; j++)
        //         {
        //             printf("%c", identArr[varCount][j]);
        //         }
        //         printf(" ");
        //         varCount++;
        //         i++;
        //     }
        //     printf("%d ", tokenArr[i]);
        // }
        varCount = 0;

//end of lexical analyser/scanner_________________________________

//begin of syntactic analyser / parser____________________________
    //printf("\n\nend scan\n begin parser:\n");

// PROGRAM
    int token = tokenArr[0];
    tokenIndex = 0; //function's tokenArray index
    //FILE* outFile = fopen("elf.txt", "w");

    block(identArr);
    token = tokenArr[tokenIndex];
    // printf("\nFinal Token:%d\n\n", token);
    if (token != periodsym)
    {
        printf("Error: program must end with period");
        exit(0);
    }

    // printing source code
        fseek(inputFile, 0, SEEK_SET);//set file pointer to beginning again
        printf("\n\nSource Program:\n");
        int printFlag = 0;
        for(int i = 0; printFlag == 0; i++)
        {
            curVal = fgetc(inputFile);//getting next value & cast to int
            if(curVal == -1)
            {
                printFlag = 1;
            }
            else
            {
                printf("%c", curVal);
            }
        }
        printf("\n\n");
        printf("No errors, program is syntactically correct\n");

    codeTable[cx].opcode = 9;
    codeTable[cx].l = 0;
    codeTable[cx].m = 3;
    cx++;
    // for(int i = 1; i < tp; i++)
    // {
    //     printf("\nkind: %d", symbolTable[i].kind);
    //     printf("\nname: %s", symbolTable[i].name);
    //     printf("\nvalue: %d", symbolTable[i].val);
    //     printf("\nlevel: %d", symbolTable[i].level);
    //     printf("\naddress: %d", symbolTable[i].addr);
    //     symbolTable[i].mark = 1;
    //     printf("\nmark: %d", symbolTable[i].mark);
    //     printf("\n");
    // }
    FILE *fptr;
    fptr = fopen("ELF.txt", "w");
    printAssembly(fptr);
    fclose(fptr);
    exit(0);
}

/*end of main____________________________________________________________________________________________________
_________________________________________________________________________________________________________________
_________________________________________________________________________________________________________________*/
void printAssembly(FILE *fptr)
{
    fprintf(fptr,"\n\nLine  OP L  M");
    for(int i = 0; i < cx; i++)
    {
        fprintf(fptr, "\n%d\t%d  %d  %d", i, codeTable[i].opcode, codeTable[i].l, codeTable[i].m);
    }
};

void block(char identArr[50][12])
{
    int tp0;//initial table index
    int tempAddr;//temporary address holder
    // printf("\ncalled block");
    // printf("|token: %d", token);
    lexlvl++;
    // printf("|lexlvl: %d", lexlvl);
    token = tokenArr[tokenIndex];
    // printf("\n||creating temp JMP||");
    tp0 = tp;//getting current index for reference later
    // printf("initial cx: %d", cx);
    tempAddr = cx;//storing current cx
    // printf("symTableAddr: %d", symbolTable[tp0].addr);
    if(firstBlock == 0)
    {
    codeTable[cx].opcode = 7;
    codeTable[cx].l = 0;
    codeTable[cx].m = 0;//temporary jump value until fixed later
    cx++;
    firstBlock = 1;
    }

    // printf("cx2: %d", cx);

    ConstDeclaration(identArr);

    int numVars = VarDeclaration(identArr);

    ProcedureDeclaration(identArr);

    printf("\n||emit fix jmp||\n");
    codeTable[tempAddr].m = (cx * 3);//fixes temp jump val
    // printf("cx3: %d", cx);
/*
code[table[tx0].adr].a=cx; // The tentative jump address is fixed up
table[tx0].adr=cx; // the space for address for the above jmp is now occupied
by the new cx
cx0=cx; gen(inc,0,dx); // inc 0,dx is generated. At run time, the space of dx is
secured
statement(lev,&tx);
gen(opr,0,0)
*/
    //emit INC (M = 3 + numVars)
    printf("\n||emit INC||");
    codeTable[cx].opcode = 6;
    codeTable[cx].l = 0;
    codeTable[cx].m = 3 + numVars;
    cx++;
    STATEMENT(identArr);
    // printf("token in block: %d", token);
    printf("\n||emit OPR||");
    codeTable[cx].opcode = 2;
    codeTable[cx].l = 0;
    codeTable[cx].m = 0;
    cx++;

    // printf("||Exit BLOCK");
    lexlvl--;
};

void insertSymbolTable(int kind, char name[11], int val, int level, int addr) //insert into symbol table
{
    // printf("\ncalled insertSymbolTable");
    symbolTable[tp].kind = kind;
    strcpy(symbolTable[tp].name, name);
    symbolTable[tp].val = val;
    symbolTable[tp].level = level;
    symbolTable[tp].addr = addr;
    symbolTable[tp].mark = 0;
    // printf("|symbol.name: %s", symbolTable[tp].name);
    // printf("|symbol.level:%d", symbolTable[tp].level);
    // printf("|symbol.addr: %d", symbolTable[tp].addr);
    tp++;
    // printf("||exit INSERTSYMTBL");
}

int symbolTableCheck(char name[11])
{
    // printf("\ncalled symbolTableCheck");
    // printf("|Token:%d", token);
    // printf("|TP:%d", tp);
    int errcount = 0;//how many dissimilarities
    int i;
    for(i = 1; i <= tp; i++)
    {
        errcount = 0;
        // printf("\n|checking name # %d", i);
        if(symbolTable[i].level <= lexlvl)
        {
            for(int j = 0; j <= (token - 1); j++)
            {
                // printf("|checking letter:%c", symbolTable[i].name[j]);
                if(symbolTable[i].name[j] != name[j])
                {
                    errcount++;
                    // printf("||error at name/letter index %d", j);
                }
                if(j == (token - 1))
                {
                    if(errcount == 0 && symbolTable[i].level <= lexlvl && symbolTable[i].mark == 0)
                    {
                        // printf("|WORD FOUND!\n");
                        // printf("||exit SYMTBLCHCK");
                        // printf("|return: %d", i);
                        return i;
                    }
                }
            }
        }
        else
        {
            //do nothing
        }

    }
    // printf("||exit SYMTBLCHCK");
    // printf("|return: -1");
    return -1;
}

void ConstDeclaration(char identArr[50][12])
{
    // printf("\ncalled ConstDeclaration");
    // printf("|Token:%d", token);
    if (token == constsym)
    {
        // printf("|constsym");
        do
        {
            tokenIndex++;
            token = tokenArr[tokenIndex];
            if (token != identsym)
            {
                printf("Error: constant keyword must be followed by identifier");
                exit(0);
            }
            // printf("|token is identsym");
            tokenIndex++;
            token = tokenArr[tokenIndex];
            // printf("|next token:%d", token);
            char tempName [11] = {'#'};
            for(int i = 0; identArr[varCount][i] != '#'; i++)
            {
                // printf("|%dletter(s) stored:", (i + 1));
                tempName[i] = identArr[varCount][i];
                // printf("%c", identArr[varCount][i]);
            }
            // printf("|name stored: %s", tempName);
            varCount++;
            // printf("varcount:%d", varCount);
            if (symbolTableCheck(tempName) != -1)
            {
                printf("Error: symbol name has already been declared");
                exit(0);
            }
            tokenIndex++;
            token = tokenArr[tokenIndex];
            if (token != eqsym)
            {
                printf("Error: constants must be assigned with =");
                exit(0);
            }
            tokenIndex++;
            token = tokenArr[tokenIndex];
            if (token != numbersym)
            {
                printf("Error: constants must be assigned an integer value");
                exit(0);
            }
            tokenIndex++;
            token = tokenArr[tokenIndex];
            insertSymbolTable(1, tempName, token, lexlvl, 0);
            tokenIndex++;
            token = tokenArr[tokenIndex];
            // printf("end of loop: %d", token);
        } while (token == commasym);
        if (token != semicolonsym)
        {
            printf("Error: constant declarations must be followed by a semicolon");
            exit(0);
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
        // printf("TokenID:%d",tokenIndex);
    }
    // printf("const not detected");
    // printf("exit CONSTDECLARATION");
};

int VarDeclaration(char identArray[50][12]) //returns number of variables
{
    // printf("\ncalled VarDeclaration");
    // printf("|Token:%d", token);
    int numVars = 0;
    if (token == varsym)
    {
        // printf("|varsym");
        do
        {
            numVars++;
            tokenIndex++;
            token = tokenArr[tokenIndex];
            if (token != identsym)
            {
                printf("Error: var keywords must be followed by identifier");
                exit(0);
            }
            tokenIndex++;
            token = tokenArr[tokenIndex];
            char tempName [11] = {'#'};
            for(int i = 0; identArray[varCount][i] != '#'; i++)
            {
                // printf("|%dletter(s), stored:", (i + 1));
                tempName[i] = identArray[varCount][i];
                // printf("%c", tempName[i]);
            }
            varCount++;
            // printf("|varcount:%d", varCount);
            if (symbolTableCheck(tempName) != -1)
            {
                printf("Error: symbol name has already been declared");
                exit(0);
            }
            insertSymbolTable(2, tempName, 0, lexlvl, numVars + 2);
            // printf("|name stored:%s", tempName);
            tokenIndex++;
            token = tokenArr[tokenIndex];
        } while (token == commasym);

        if (token != semicolonsym)
        {
            printf("Error: variable declarations must be followed by a semicolon");
            exit(0);
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
    }
    // printf("||exit VARDECLARATION");
    return(numVars);
};

void ProcedureDeclaration(char identArray[50][12])
{
    // printf("\ncalled PROCEDUREDECLARATION");
    // printf("|token: %d", token);
    if (token == procsym)
    {
        do
        {
            // printf("|Procedure loop");
            tokenIndex++;
            token = tokenArr[tokenIndex];
            if (token != identsym)
            {
                printf("Error: Incorrect symbol after procedure declaration");
                exit(0);
            }
            int jmpIndex = cx;//creating jmp for procedure
            codeTable[cx].opcode = 7;
            codeTable[cx].l = 0;
            codeTable[cx].m = 0;
            cx++;

            tokenIndex++;
            token = tokenArr[tokenIndex];
            char tempName [11] = {'#'};
            for(int i = 0; identArray[varCount][i] != '#'; i++)
            {
                // printf("|%dletter(s), stored:", (i + 1));
                tempName[i] = identArray[varCount][i];
                // printf("%c", tempName[i]);
            }
            varCount++;
            // printf("|varcount:%d", varCount);
            insertSymbolTable(3, tempName, 0, lexlvl, cx);
            // printf("toke: %d", token);
            

            tokenIndex++;
            token = tokenArr[tokenIndex];
            if (token != semicolonsym)
            {
                printf("Error: procedure declarations must be followed by a semicolon");
                exit(0);
            }

            tokenIndex++;
            token = tokenArr[tokenIndex];
            block(identArray);
            if (token != semicolonsym)
            {
                printf("Error: procedure declarations must be followed by a semicolon");
                exit(0);
            }
            codeTable[jmpIndex].m = cx;
            // printf("|semicolonsym in procedure");
            tokenIndex++;
            token = tokenArr[tokenIndex];
            // printf("|token at end of procedure declaration: %d", token);
            // printf("Exit PROCEDUREDECLARATION");
        }   while (token == procsym);
        /*
        EBNF: procedure-declaration ::= { "procedure" ident ";" block ";" }
        while TOKEN = "procsym" do begin
    GET(TOKEN);
    if TOKEN != “identsym” then ERROR;
    GET(TOKEN);
    if TOKEN != "semicolomsym" then ERROR;
    GET(TOKEN);
    BLOCK;
    if TOKEN != "semicolomsym" then ERROR;
    GET(TOKEN)
    end;
    */
    }
    // printf("||No procedures detected");
};


int STATEMENT(char identArray[50][12])
{
    // printf("\ncalled STATEMENT");
    // printf("|Token:%d", token);
    int symIdx, jpcIdx, loopIdx;

    if (token == identsym)
    {
        // printf("|identsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        char tempName [11] = {'#'};
        for(int i = 0; identArray[varCount][i] != '#'; i++)
        {
            // printf("|%dletter(s), stored:", (i + 1));
            tempName[i] = identArray[varCount][i];
            // printf("%c", tempName[i]);
        }
        // printf("|name stored:%s", tempName);
        varCount++;
        symIdx = symbolTableCheck(tempName);
        // printf("|Current symidx: %d", symIdx);
        if (symIdx == -1)
        {
            printf("Error: undeclared identifier");
            exit(0);
        }
        if (symbolTable[symIdx].kind != 2 /*(not a var)*/)
        {
            printf("Error: only variable values may be altered");
            exit(0);
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
        // printf("Token should be 20:%d", token);

        if (token != becomessym)
        {
            // printf("|token:%d", token);
            printf("Error: assignment statements must use :=");
            exit(0);
        }
        // printf("|becomesym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        finalToken = tokenArr[tokenIndex];
        EXPRESSION(identArray);
        // emit STO (M = table[symIdx].addr)
        printf("\n||emit STO||");
        codeTable[cx].opcode = 4;
        codeTable[cx].l = lexlvl - symbolTable[symIdx].level;
        codeTable[cx].m = symbolTable[symIdx].addr;
        cx++;
        return(0);
    }
    else if(token == callsym)
    {
        // printf("|callsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        if(token != identsym)
        {
            printf("Error: identifier must follow call");
            exit(0);
        }
        else
        {
            tokenIndex++;
            token = tokenArr[tokenIndex];
            char tempName [11] = {'#'};
            for(int i = 0; identArray[varCount][i] != '#'; i++)
            {
                // printf("|%dletter(s), stored:", (i + 1));
                tempName[i] = identArray[varCount][i];
                // printf("%c", tempName[i]);
            }
            // printf("|name stored:%s", tempName);
            varCount++;
            symIdx = symbolTableCheck(tempName);
            if (symIdx == -1)
            {
                printf("Error: undeclared identifier");
                exit(0);
            }
            if(symbolTable[symIdx].kind == 3)
            {
                //gen(CAL, level – symbollevel(i), symboladdr(i));
                printf("\n||emit CAL||");
                codeTable[cx].opcode = 5;
                codeTable[cx].l = lexlvl - symbolTable[symIdx].level;//needs to be updated
                codeTable[cx].m = symbolTable[symIdx].addr;
                cx++;
            }
            else
            {
                printf("Error: call must be followed by a procedure identifier");
                exit(0);
            }
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
    }
    else if (token == beginsym)
    {
        // printf("|beginsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        STATEMENT(identArray);
        do
        {
            tokenIndex++;
            token = tokenArr[tokenIndex];
            STATEMENT(identArray);
            // printf("|whiletoken:%d", token);
        }while (token == semicolonsym);
    
        // tokenIndex ++;
        // token = tokenArr[tokenIndex];
        // printf("|token:%d", token);

        if (token != endsym)
        {
            printf("Error: begin must be followed by end");
            exit(0);
        }
        // printf("|endsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        // printf("//token before return: %d", token);
        // printf("\n||Exit STATEMENT");
        return(0);
    }
    else if (token == ifsym)
    {   
        // printf("|ifsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        CONDITION(identArray);
        // jpcIdx = current code index
        // emit JPC
        printf("\n||emit JPC");
        jpcIdx = cx;
        codeTable[cx].opcode = 8;
        codeTable[cx].l = 0;
        codeTable[cx].m = 0;
        cx++;
        if (token != thensym)
        {
            // printf("|thensym");
            printf("Error: if must be followed by then");
            exit(0);
        }
        // printf("|thensym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        STATEMENT(identArray);
        // printf("|then statement done");
        // printf("|token %d", token);
        // code[jpcIdx].M = current code index
        codeTable[jpcIdx].m = cx;
        if(token != fisym)
        {
            printf("Error: then must be followed by fi");
            exit(0);
        }
        // printf("|fisym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        // printf("\n||Exit STATEMENT");
        return(0);
    }
    else if (token == whilesym)
    {
        // printf("|whilesym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        // loopIdx = current code index
        loopIdx = cx;
        CONDITION(identArray);
        if (token != dosym)
        {
            printf("Error: while must be followed by do");
            exit(0);
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
        // jpcIdx = current code index
        // emit JPC
        printf("\n||emit JPC(2)");
        jpcIdx = cx;
        codeTable[cx].opcode = 8;
        codeTable[cx].l = 0;
        codeTable[cx].m = jpcIdx;
        cx++;
        STATEMENT(identArray);
        // emit JMP (M = loopIdx)
        // code[jpcIdx].M = current code index
        printf("\n||emit JMP(2)");
        codeTable[cx].opcode = 7;
        codeTable[cx].m = loopIdx;
        cx++;
        codeTable[jpcIdx].m = cx;
        return(0);
    }
    else if (token == readsym)
    {
        // printf("|readsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        // printf("|token: %d", token);
        if (token != identsym)
        {
            printf("Error: const, var, and read keywords must be followed by identifier");
            exit(0);
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
        char tempName [11] = {'#'};
        for(int i = 0; identArray[varCount][i] != '#'; i++)
        {
            // printf("|%dletter(s), stored:", (i + 1));
            tempName[i] = identArray[varCount][i];
            // printf("%c", tempName[i]);
        }
        // printf("|name stored:%s", tempName);
        symIdx = symbolTableCheck(tempName);
        // printf("|Current symidx: %d", symIdx);

        if (symIdx == -1)
        {
            printf("Error: undeclared identifier");
            exit(0);
        }
        if (symbolTable[symIdx].kind != 2 /*(not a var)*/)
        {
            printf("Error: only variable values may be altered");
            exit(0);
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
        // emit READ
        printf("\n||emit READ||");
        codeTable[cx].opcode = 9;
        codeTable[cx].l = 0;
        codeTable[cx].m = 2;
        cx++;
        // emit STO (M = table[symIdx].addr)
        printf("\n||emit STO(2)||");
        codeTable[cx].opcode = 4;
        codeTable[cx].l = lexlvl - symbolTable[symIdx].level;
        codeTable[cx].m = symbolTable[symIdx].addr;
        cx++;
        return(0);
    }
    else if (token == writesym)
    {
        // printf("|writesym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        EXPRESSION(identArray);
        // emit WRITE
        printf("\n||emit WRITE||");
        codeTable[cx].opcode = 9;
        codeTable[cx].l = 0;
        codeTable[cx].m = 1;
        cx++;
        return(0);
    }
    // printf("||exit STATEMENT");
};

void CONDITION(char identArray[50][12])
{
    // printf("\ncalled CONDITION");
    // printf("|Token:%d", token);
    if (token == oddsym)
    {
        // printf("|oddsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        EXPRESSION(identArray);
        // emit ODD
        printf("\n||emit ODD||");
        codeTable[cx].opcode = 2;
        codeTable[cx].l = 0;
        codeTable[cx].m = 11;
        cx++;

    }
    else
    {
        EXPRESSION(identArray);
        if (token == eqsym)
        {
            // printf("|eqsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
            EXPRESSION(identArray);
            // emit EQL
        printf("\n||emit EQL||");
            codeTable[cx].opcode = 2;
            codeTable[cx].l = 0;
            codeTable[cx].m = 5;
            cx++;
        }
        else if (token == neqsym)
        {
            // printf("|neqsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
            EXPRESSION(identArray);
            // emit NEQ
        printf("\n||emit NEQ||");
            codeTable[cx].opcode = 2;
            codeTable[cx].l = 0;
            codeTable[cx].m = 6;
            cx++;
        }
        else if (token == lessym)
        {
            // printf("|lessym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
            EXPRESSION(identArray);
            // emit LSS
        printf("\n||emit LSS||");
            codeTable[cx].opcode = 2;
            codeTable[cx].l = 0;
            codeTable[cx].m = 7;
            cx++;
        }
        else if (token == leqsym)
        {
            // printf("|leqsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
            EXPRESSION(identArray);
            // emit LEQ
        printf("\n||emit LEQ||");
            codeTable[cx].opcode = 2;
            codeTable[cx].l = 0;
            codeTable[cx].m = 8;
            cx++;
        }
        else if (token == gtrsym)
        {
            // printf("|gtrsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
            EXPRESSION(identArray);
            // emit GTR
        printf("\n||emit GTR||");
            codeTable[cx].opcode = 2;
            codeTable[cx].l = 0;
            codeTable[cx].m = 9;
            cx++;
        }
        else if (token == geqsym)
        {
            // printf("|geqsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
            EXPRESSION(identArray);
            // emit GEQ
        printf("\n||emit GEQ||");
            codeTable[cx].opcode = 2;
            codeTable[cx].l = 0;
            codeTable[cx].m = 10;
            cx++;
        }
        else
        {
            printf("Error: condition must contain comparison operator");
            exit(0);
        }
    }
    // printf("||Exit CONDITION");
};
void EXPRESSION(char identArray[50][12])//(HINT: modify it to match the grammar)
{
    // printf("\ncalled EXPRESSION");
    // printf("|Token:%d", token);
    TERM(identArray);

    if (token == minussym)
    {
        // printf("|minussym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        TERM(identArray);  
        while (token == plussym || token == minussym)
        {
            if (token == plussym)
            {
                tokenIndex++;
                token = tokenArr[tokenIndex];
                TERM(identArray);
                // emit ADD
                printf("\n||emit ADD||");
                codeTable[cx].opcode = 2;
                codeTable[cx].l = 0;
                codeTable[cx].m = 1; 
                cx++;   
            }
            else
            {
                // printf("/nElse in EXPRESSION-minus");
                tokenIndex++;
                token = tokenArr[tokenIndex];
                TERM(identArray);
                // emit SUB
                printf("\n||emit SUB||");
                codeTable[cx].opcode = 2;
                codeTable[cx].l = 0;
                codeTable[cx].m = 2;  
                cx++;
            }
        }
    }
    else if (token == plussym)
    {
        // printf("|plussym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        TERM(identArray);
        // printf("\nToken after Term in plussym expr: %d", token);
        while (token == plussym || token == minussym)
        {   
            // printf("|While in plus/minus loop in expr");
            // printf("|Token in loop: %d", token);
            if (token == plussym)
            {
                // printf("|if plussym");
                tokenIndex++;
                token = tokenArr[tokenIndex];
                TERM(identArray);
                // emit ADD
                printf("\n||emit ADD||");
                codeTable[cx].opcode = 2;
                codeTable[cx].l = 0;
                codeTable[cx].m = 1;   
                cx++;
            }
            else
            {
                // printf("/nElse in EXPRESSION-plus");
                tokenIndex++;
                token = tokenArr[tokenIndex];
                TERM(identArray);
                // emit SUB
                printf("\n||emit SUB||");
                codeTable[cx].opcode = 2;
                codeTable[cx].l = 0;
                codeTable[cx].m = 2;  
                cx++;
            }
        }
    }
    // printf("||exit EXPRESSION");

};

void TERM(char identArray[50][12])
{
    // printf("\ncalled TERM");
    // printf("|Token:%d", token);//current token
    FACTOR(identArray);
    // printf("\nFACTOR->TERM loop");
    // printf("|Token:%d", token);
    while (token == multsym || token == slashsym)
    {
        if (token == multsym)
        {
            // printf("|multsym");
            tokenIndex++;
            token = tokenArr[tokenIndex];
            FACTOR(identArray);
            // emit MUL
        printf("\n||emit MUL||");
        codeTable[cx].opcode = 2;
        codeTable[cx].l = 0;
        codeTable[cx].m = 3;
        cx++;
        }
        else if (token == slashsym)
        {
            // printf("|slashsym");
            tokenIndex++;
            token = tokenArr[tokenIndex];
            FACTOR(identArray);
            // emit DIV
            printf("\n||emit DIV||");
            codeTable[cx].opcode = 2;
            codeTable[cx].l = 0;
            codeTable[cx].m = 4;
            cx++;
        }
        else
        {
            // printf("\nTerm Else");
            tokenIndex++;
            token = tokenArr[tokenIndex];
            FACTOR(identArray);
            // emit MOD
        }
}
// printf("||Exit loop/TERM");
};

void FACTOR(char identArray[50][12])
{
    // printf("\ncalled FACTOR");
    // printf("|Token:%d", token);
    int symIdx = 0;
    if ((tokenArr[tokenIndex]) == identsym)
    {
        // printf("|identsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        char tempName [11] = {'#'};
        for(int i = 0; i < token; i++)
        {
            // printf("|%dletter(s), stored:", (i + 1));
            tempName[i] = identArray[varCount][i];
            //printf("%c", tempName[i]);
        }
        // printf("|name stored:%s", tempName);
        varCount++;
        symIdx = symbolTableCheck(tempName);
        // printf("|Current symidx: %d", symIdx);
        if (symIdx == -1)
        {
            printf("Error: undeclared identifier");
            exit(0);
        }
        if (symbolTable[symIdx].kind == 1) //(const)
        {
            // emit LIT (M = table[symIdx].Value)
        printf("\n||emit LIT||");
        // printf("|Symidx: %d", symIdx);
        // printf("|tp: %d", tp);
        codeTable[cx].opcode = 1;
        codeTable[cx].l = 0;
        codeTable[cx].m = symbolTable[symIdx].val;
        cx++;
        }
        else //(var)
        {
            // emit LOD (M = table[symIdx].addr)
        printf("\n||emit LOD||");
        // printf("|Symidx: %d", symIdx);
        // printf("|tp: %d", tp);
        codeTable[cx].opcode = 3;
        codeTable[cx].l = 0;
        codeTable[cx].m = symbolTable[symIdx].addr;
        cx++;
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
    }
    else if ((tokenArr[tokenIndex]) == numbersym)
    {   
        // printf("|numbersym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        // printf("|token after numbersym: %d", token);
        symbolTable[tp - 1].val = token;
        //emit LIT (M = table[symIdx].Value)
        printf("\n||emit LIT(2)||");
        // printf("|tp: %d", tp);
        codeTable[cx].opcode = 1;
        codeTable[cx].l = 0;
        codeTable[cx].m = symbolTable[tp - 1].val;
        cx++;

        tokenIndex++;
        token = tokenArr[tokenIndex];
    }
    else if (token == lparentsym)
    {
        // printf("|lparentsym");
        tokenIndex++;
        token = tokenArr[tokenIndex];
        EXPRESSION(identArray);
        if (token != rparentsym)
        {
            // printf("|rparentsym");
            printf("Error: right parenthesis must follow left parenthesis");
            exit(0);
        }
        tokenIndex++;
        token = tokenArr[tokenIndex];
    }
    else
    {
        printf("Error: arithmetic equations must contain operands, parentheses, numbers, or symbols");
        exit(0);
    }
    // printf("||Exit FACTOR");
};
