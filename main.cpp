/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: CindyHan
 *
 * Created on 2017年4月3日, 下午 2:34
 */

#include <cstdlib>
#include <stdio.h>
#include <string.h> 
#include <ctype.h>

using namespace std;

enum TokenType { IDENTIFIER = 34512, CONSTANT = 87232, SPECIAL = 29742, ANNOTATION = 66666 } ;

typedef char * CharPtr ;

struct Column {
  int column ;                  // 此token(的字首)在本line有出現於此column
  Column * next;                // 下一個此token(在本line)有出現的column
} ; // Column

typedef Column * ColumnPtr ;

struct Line {                   // struct Line 記錄了4種資訊
  int line ;                    // 此token放在哪一 line
  ColumnPtr firstAppearAt ;     // 指到此token第一次出現在此line的哪一column
  ColumnPtr lastAppearAt ;      // 指到此token最後出現在此line的哪一column
  Line *  next;                 // 同一 token 下一次出現在哪一 line
} ;

typedef Line * LinePtr ;

struct Token {                  // Struct token 紀錄了4種資訊
  CharPtr tokenStr ;            // tokenStr 放你切下來的token
  TokenType type ;              // type 紀錄此token是屬於哪一個case
  LinePtr firstAppearOn ;       // 此 token 第一次出現在哪一 line
  LinePtr lastAppearOn ;        // 此 token 最後出現在哪一 line
  Token * next ;                // 指向下一個token
} ; // struct Token

typedef Token * TokenPtr ;

TokenPtr gFront = NULL, gRear = NULL ;  // 分別指向Token串列的頭跟尾
typedef char Str100[100] ;   // 此型別是打算用來宣告一個陣列變數、以供讀字串之用
int gLine = 1 ;              // 「下一個要讀進來的字元」所在的line number
int gColumn = 1 ;            // 「下一個要讀進來的字元」所在的column number

// function區 ==================================================================
CharPtr GetToken( TokenPtr &tempToken, char &input, int &tempLine, int &tempColumn );
void TokenType( Str100 &tokenStr, TokenPtr &tempToken, char &input ); 
bool Identifier( Str100 &tokenStr, char &input );
bool Constant( Str100 &tokenStr, char &input ); 
bool Special( Str100 &tokenStr, char &input ); 
bool Annotation( Str100 &tokenStr, TokenPtr &tempToken, char &input );
void AnnotationType2( char &input  ); 
void OrderInsert( TokenPtr &root, TokenPtr tempToken, int tempLine, int tempColumn  );
void InsertOnSameLine( ColumnPtr &tail, ColumnPtr &head, int column );
void InsertOnDifferentLine( LinePtr &tail, LinePtr &head, int line, int column );
void PrintAll( TokenPtr head, int &firstToken );
void PrintLine( LinePtr firstAppearOn );
void PrintColumn( ColumnPtr firstAppearAt, int tempLine );
void Command();
void HowManyToken( TokenPtr head, int &i );
void CaseNum( TokenPtr head, int &case1, int &case2, int &case3 );
void FindAndPrint( TokenPtr head, Str100 inputToken );
void OnLineAndPrint( TokenPtr gFront, int line );
bool FindLine( LinePtr Line, int line );
void PrintSpace( int num );
// function區 ==================================================================

int main() {  
    TokenPtr tempToken;
    TokenPtr root = NULL;
    tempToken = new Token;
    int tempLine = 0, tempColumn = 0, firstToken = 0;
    char input = '\0';
    
    tempToken->tokenStr = GetToken( tempToken, input, tempLine, tempColumn );
    while( strcmp( tempToken->tokenStr, "END_OF_FILE" ) != 0 ) {
        OrderInsert( root, tempToken, tempLine, tempColumn );
        tempToken = new Token;
        tempToken->tokenStr = GetToken( tempToken, input, tempLine, tempColumn ); 
    } // while
    
    gFront = root;
    printf( "\n" );
    PrintAll( root, firstToken );
    
    printf( "\n\n" );
    Command();
    
    return 0;
} // main()

CharPtr GetToken( TokenPtr &tempToken, char &input, int &tempLine, int &tempColumn ) {   // Get一個token、判斷type、判斷line及column
    Str100 tokenStr = { 0 } ;
    CharPtr returnTokenStr = NULL ;

    int i = 0;

    if ( !isspace(input) && input != '\0' ) {
        tempLine = gLine;
        tempColumn = gColumn;
        TokenType( tokenStr, tempToken, input );
    } // if
    
    else {
        for ( i = 0; isspace( input ) || input == '\0'; input = getchar() ) {
           
            if ( input == '\n' ) {
                gLine ++;
                gColumn = 1;
            } // if 

            else if ( input != '\0' )
                gColumn ++; 
            
        } // for()
        // 出迴圈的字元是遇到的第一個非(white字元，此時i=0)
        // 接下來要判斷為什麼type、是否為註解
       
        tempLine = gLine;
        tempColumn = gColumn;
        TokenType( tokenStr, tempToken, input );
    } // else 
            
    returnTokenStr = new char[ strlen( tokenStr ) + 1 ] ;
    strcpy( returnTokenStr, tokenStr ) ;

    return returnTokenStr ;
} // GetToken()

void TokenType( Str100 &tokenStr, TokenPtr &tempToken, char &input ) {
    // 判斷此token是什麼case(identifier/constat/special/annotation)
    // 分別把剩下的字元讀進來

    if ( Identifier( tokenStr, input ) )
        tempToken->type = IDENTIFIER;
    else if ( Constant( tokenStr, input ) )
        tempToken->type = CONSTANT;
    else if ( Annotation( tokenStr, tempToken, input ) )
        tempToken->type = ANNOTATION;
    else if ( Special( tokenStr, input ) )
        tempToken->type = SPECIAL;
    
    return;
} // TokenType()

bool Identifier( Str100 &tokenStr, char &input ) {
    // 判斷是否為identifier
    // 把剩下字元讀進來
    
    int i = 0;
    if ( isalpha(input) || input == '_' ) {
        while( isdigit(input) || isalpha(input) || input == '_' ) {
            tokenStr[i] = input;
            i ++;
            input = getchar();
            gColumn ++;
        } // while
        
        return true;
    } // if 

    return false;
} // Identifier()

bool Constant( Str100 &tokenStr, char &input ) {
    // 判斷是否為constant
    // 把剩下字元讀進來    
    
    int i = 0;
    if ( isdigit(input) ) {
        while( isdigit(input) || input == '.' ) {            
            tokenStr[i] = input;
            i ++;
            input = getchar();
            gColumn ++;
        } // while
        
        return true;   
    } // if()
    
    else if ( input == '"' ) {
        tokenStr[i] = input;
        input = getchar();
        gColumn ++;
        while( input != '"' ) {
            i ++;
            tokenStr[i] = input;
            input = getchar();
            gColumn ++;
        } // while
        
        // 這裡出來input會是"，所以也要放進tokenStr[i]裡面
        i ++;
        tokenStr[i] = input;
        input = getchar();
        gColumn ++;
        
        return true;    
    } // else if
    
    else if ( input == '\'' ) {
        tokenStr[i] = input;
        input = getchar();
        gColumn ++;
        
        if ( input == '\\' ) {
            i ++;
            tokenStr[i] = input;
            input = getchar();
            gColumn ++;
            
            if ( input == '\'' ) {
                i ++;
                tokenStr[i] = input;  
                
                // 把最後一個'讀進來然後放進字串
                input = getchar();
                gColumn ++;
                i ++;
                tokenStr[i] = input;
                
                // 再多讀一個字元回到GetToken()
                input = getchar();
                gColumn ++;
                return true;
            } // if
            
            else {
                while( input != '\'' ) {
                    i ++;
                    tokenStr[i] = input;
                    input = getchar();
                    gColumn ++;   
                } // while
                
                // 這裡出來的input會是'，所以要放到字串裡面
                i ++;
                tokenStr[i] = input;
                input = getchar();
                gColumn ++;
                return true;
            } // else
        } // if 
        
        else {
            while( input != '\'' ) {
                i ++;
                tokenStr[i] = input;
                input = getchar();
                gColumn ++;   
            } // while
            
            // 這裡出來的input會是'，所以要放到字串裡面
            i ++;
            tokenStr[i] = input;
            input = getchar();
            gColumn ++;
            return true;   
        } // else if
    } // else if
    
    return false;
} // Constant()

bool Special( Str100 &tokenStr, char &input ) {
    // 判斷是否為special
    // 特殊情況討論
    
    int i = 0;
    if ( ispunct(input) ) {
        tokenStr[i] = input;
        
        if( input == '+' ) {
            input = getchar();
            gColumn ++;
            
            if ( input == '+' ) {
                i ++;
                tokenStr[i] = input;
            } // if ++
            
            else return true;
        } // if ++
        
        else if ( input == '-' ) {
            input = getchar();
            gColumn ++;
            
            if ( input == '-' ) {
                i ++;
                tokenStr[i] = input;
            } // if --
            
            else if ( input == '>' ) {
                i ++;
                tokenStr[i] = input;
            } // else if ->
            
            else return true;
        } // else if -- or ->
        
        else if ( input == '>' ) {
            input = getchar();
            gColumn ++;
            
            if ( input == '=' ) {
                i ++;
                tokenStr[i] = input;
            } // if >=
            
            else if ( input == '>' ) {
                i ++;
                tokenStr[i] = input;
            } // else if >>
            
            else return true;
        } // else if >=
        
        else if ( input == '<' ) {
            input = getchar();
            gColumn ++;
            
            if ( input == '=' ) {
                i ++;
                tokenStr[i] = input;
            } // if <=
            
            else if ( input == '<' ) {
                i ++;
                tokenStr[i] = input;
            } // else if <<      
            
            else return true;
        } // else if <=     

        else if ( input == '=' ) {
            input = getchar();
            gColumn ++;
            
            if ( input == '=' ) {
                i ++;
                tokenStr[i] = input;
            } // if ==
            
            else return true;
        } // else if ==       
        
        else if ( input == '!' ) {
            input = getchar();
            gColumn ++;
            
            if ( input == '=' ) {
                i ++;
                tokenStr[i] = input;
            } // if !=
            
            else return true;
        } // else if !=

        else if ( input == '|' ) {
            input = getchar();
            gColumn ++;
            
            if ( input == '|' ) {
                i ++;
                tokenStr[i] = input;
            } // if ||
            
            else return true;
        } // else if ||
        
        else if ( input == '&' ) {
            input = getchar();
            gColumn ++;
            
            if ( input == '&' ) {
                i ++;
                tokenStr[i] = input;
            } // if &&
            
            else return true;
        } // else if &&

        
        input = getchar();
        gColumn ++;
        
        return true;
    } // if
    
    else return false;       
} // Special()

bool Annotation( Str100 &tokenStr, TokenPtr &tempToken, char &input ) {
    // 判斷是否為annotation
    // 把剩下的字元讀進來
    char temp = input;
    
    if ( input == '/' ) {
        input = getchar();
        gColumn ++;
    
        // 第一個字元是'/'有兩種情況
        // 一、註解ype1 二、註解type2
        
        // type 1
        if ( input == '/' ) {
            while ( input != '\n' ) {
                input = getchar();
                gColumn ++;
            } // while    

            return true;
        } // if

        // type 2
        else if ( input == '*' ) {
            AnnotationType2( input );
            return true;
        } // else if
        
        else { 
            tokenStr[0] = temp;
            tempToken->type = SPECIAL;
            return false;
        } // else
    } // if
    
    // not an annotation    
    return false;
        
} // Annotation()

void AnnotationType2( char &input  ) {
    input = getchar();
    gColumn ++;
    
    while ( input != '*' ) {
        input = getchar();
        gColumn ++;
        if ( input == '\n' ) {
            gLine ++;
            gColumn = 1;
        } // if 
    } // while
    
    input = getchar();
    gColumn ++;
    if ( input == '/' )
        return;
    else 
        AnnotationType2( input );
} // AnnotationType2

void OrderInsert( TokenPtr &root, TokenPtr tempToken, int tempLine, int tempColumn  ) {
    
    // 先找是否有一樣的
    // 有一樣 找是否有同行
    // 有同行 排列column
    if ( tempToken->type == ANNOTATION ) return;
    
    if ( root == NULL ) {
        root = tempToken;
        root->firstAppearOn = new Line;
        root->firstAppearOn->next = NULL;
        root->firstAppearOn->line = tempLine;
        root->firstAppearOn->firstAppearAt = new Column;
        root->firstAppearOn->firstAppearAt->column = tempColumn;
        root->firstAppearOn->firstAppearAt->next = NULL;
        root->firstAppearOn->lastAppearAt = root->firstAppearOn->firstAppearAt;
        root->lastAppearOn = root->firstAppearOn;       
        root->next = NULL;
        gRear = root;
        
        return;
    } // if 
            
    else {
        if ( strcmp( root->tokenStr, tempToken->tokenStr ) == 0 ) {
            if ( root->firstAppearOn->line == tempLine ) {
                InsertOnSameLine( root->firstAppearOn->lastAppearAt, 
                root->firstAppearOn->firstAppearAt, tempColumn );
                return;
            } // if 如果在同一行
            
            else {
                InsertOnDifferentLine( root->lastAppearOn, root->firstAppearOn,
                tempLine, tempColumn );
                return;
            } // else 如果在不同行
            
        } // if 如果有同樣的token
        
        else if ( strcmp( root->tokenStr, tempToken->tokenStr ) > 0 ) {
            tempToken->next = root;
            root = tempToken;
            
            root->firstAppearOn = new Line;
            root->firstAppearOn->next = NULL;
            root->firstAppearOn->line = tempLine;
            root->firstAppearOn->firstAppearAt = new Column;
            root->firstAppearOn->firstAppearAt->column = tempColumn;
            root->firstAppearOn->firstAppearAt->next = NULL;
            root->firstAppearOn->lastAppearAt = root->firstAppearOn->firstAppearAt;
            root->lastAppearOn = root->firstAppearOn;   
            
            return;
        } // else 
          
        OrderInsert( root->next, tempToken, tempLine, tempColumn );
            
    } // else 
        
} // OrderInsert()
      
void InsertOnSameLine( ColumnPtr &tail, ColumnPtr &head, int column ) {
    if( head == NULL ) {
        head = new Column;
        head->column = column;
        head->next = NULL;
        
        tail = head;
        return;
    } // if 
    
    else
        InsertOnSameLine( tail, head->next, column );
} // InsertOnSameLine

void InsertOnDifferentLine( LinePtr &tail, LinePtr &head, int line, int column ) {
    if( head == NULL ) {
        head = new Line;
        head->line = line;
        head->next = NULL;
        
        head->firstAppearAt = new Column;
        head->firstAppearAt->column = column;
        head->lastAppearAt = head->firstAppearAt;
        head->firstAppearAt->next = NULL;
        
        tail = head;
        return;
    } // if
    
    else
        InsertOnDifferentLine( tail, head->next, line, column );
} // InsertOnDifferentLine()

void PrintAll( TokenPtr head, int &firstToken ) {
   
    if ( head != NULL ) {
        
        if ( firstToken != 0 ) {
            printf( "\n" );
        } // if
        
        firstToken = 1;
        printf( "%s ", head->tokenStr );
        PrintLine( head->firstAppearOn );
        PrintAll( head->next, firstToken );
    } // if 
    
    else return;
} // PrintAll()

void PrintLine( LinePtr firstAppearOn ) {
    
    if ( firstAppearOn != NULL ) {
        int tempLine = firstAppearOn->line;
        PrintColumn( firstAppearOn->firstAppearAt, tempLine );
        PrintLine( firstAppearOn->next );
    } // if
    
    else return;
    
} // PrintLine()

void PrintColumn( ColumnPtr firstAppearAt, int tempLine ) {
    
    if ( firstAppearAt != NULL ) {
        printf( "(%d,%d)", tempLine, firstAppearAt->column );
        PrintColumn( firstAppearAt->next, tempLine );
    } // if
    
    else return;
} // PrintColumn()

void Command() {
    int input = 0, total = 0;
    int case1 = 0, case2 = 0, case3 = 0;

    Str100 inputToken = { 0 };
    
    printf( "請輸入指令：\n" );
    printf( "1.總共有多少種 token\n" );
    printf( "2.三種case各有多少 token\n" );
    printf( "3.印出指定 token 的位置 (line number,column number) (要排序)\n" );
    printf( "4.印出所指定的那一 line 出現哪幾個 token (要排序)\n" );
    printf( "5.結束\n" );
    printf( "\n" );
    
    scanf( "%d", &input );
    
    if ( input == 5 ) {
        printf( "byebye" );
        return;
    } // if
    
    else {
        if ( input == 1 ) {
            total = 0;
            HowManyToken( gFront, total );
            printf( "總共%d種\n", total );
        } // if ( input == 1 )
        
        else if ( input == 2 ) {
            case1 = 0, case2 = 0, case3 = 0;
            CaseNum( gFront, case1, case2, case3 );
            printf( "Case1" );
            PrintSpace( case1 );
            printf( "Case2" );
            PrintSpace( case2 );
            printf( "Case3" );
            PrintSpace( case3 );
        } // else if ( input == 2 )
        
        else if ( input == 3 ) {
            printf( "請輸入要搜尋的Token :\n" );
            scanf( "%s", inputToken );
            FindAndPrint( gFront, inputToken );
        } // else if
        
        else if ( input == 4 ) {
            printf( "請輸入指定的Line :\n" );
            scanf( "%d", &input );
            OnLineAndPrint( gFront, input );
        } // else if

        printf( "\n" );
        Command();
    } // else
} // Command

void HowManyToken( TokenPtr head, int &i ) {
    
    if ( head != NULL ) {
        i ++;
        HowManyToken( head->next, i );
    } // if
    
    else return;
} // HowManyToken()

void CaseNum( TokenPtr head, int &case1, int &case2, int &case3 ) {
    
    if( head != NULL ) {
        if ( head->type == IDENTIFIER ) case1 ++;
        else if ( head->type == CONSTANT ) case2 ++;
        else if ( head->type == SPECIAL ) case3 ++;
        
        CaseNum( head->next, case1, case2, case3 );
    } // if 
    
    else return;
} // CaseNum()

void FindAndPrint( TokenPtr head, Str100 inputToken ) {
    if ( head != NULL ) {
        if ( strcmp( head->tokenStr, inputToken ) == 0 ) {
            printf( "%s ", inputToken );
            PrintLine( head->firstAppearOn );
            printf( "\n" );
            return;
        } // if
        
        FindAndPrint( head->next, inputToken );
    } // if
    
    else 
        printf( "查無此token : %s\n", inputToken );
    return;
} // FindAndPrint()

void OnLineAndPrint( TokenPtr head, int line ) {
    if ( head != NULL ) {
        if ( FindLine( head->firstAppearOn, line ) )
            printf( "%s\n", head->tokenStr );
        
        OnLineAndPrint( head->next, line );
    } // if
    
    else return;
} // OnLineAndPrint()

bool FindLine( LinePtr Line, int line ) {
    if ( Line != NULL ) {
        if( Line->line == line ) {
            return true;
        } // if
        
        return FindLine( Line->next, line );
    } // if
    
    else return false;
} // FindLine()

void PrintSpace( int num ) {
    if( num < 10 )
        printf( "共    %d個\n", num );
    else if( num < 100 && num >= 10 )
        printf( "共   %d個\n", num );
    else if( num < 1000 && num >= 100 )
        printf( "共  %d個\n", num );
    else if( num < 10000 && num >= 1000 )
        printf( "共 %d個\n", num );
    else if( num < 100000 && num >= 10000 )
        printf( "共%d個\n", num );
    
    return;
} // PrintSpace