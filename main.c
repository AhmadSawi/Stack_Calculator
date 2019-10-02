/*
 * Birzeit University
 * To: Dr. Majdi Mafarja
 * Ahmad Sawi
 * 1150007
 * Data Structures Project 2
 * */

/*
 * Summary: this is a program that works in a simiilar way to a calculator but uses the stack to find all the
 *          answers and compares them to another equation to see if they have the same value of not.
 *          this program focuses alot on the stack data structure and implements most things using it
 * */

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define DATA_STRING_LENGTH 15               //macros for flexielity in adjusting code later
#define SCANNED_STRING_MAX_LENGTH 100
#define EQUATION_LENGTH 50
#define FILE_NAME_LENGTH 30

struct node{
    char data[DATA_STRING_LENGTH];
    struct node *next;
};

typedef struct node node_t;
typedef node_t *p2n;
typedef p2n StackPointer;
typedef p2n Stack;
typedef p2n List;         //using the same node for stacks and lists
typedef p2n Position;

bool isEmpty(Stack);                                          //function that tells if a stack is empty
Stack createHead();                                           //makes a new head. works for both stacks and lists
StackPointer newNode(char[]);                                 //makes a new node for use with lists and stacks
void push(Stack, char[]);                                     //pushes an element to the stack
void top(Stack, char[]);                                      //returns the top elemet of the stack
void pop(Stack);                                              //removes the top element from the stack
bool insertLast(List, char[]);                                //inserts a new node to a list
int fileLineCount(FILE*);                                     //counts how many lines in a file
void getFromFile(FILE*, char[][SCANNED_STRING_MAX_LENGTH]);   //gets data from file
void removeSpaces(char[]);                                    //removes all spaces from a string
void removeNewLine(char[]);                                   //removes the new line character
void removeEmptylines(char[][SCANNED_STRING_MAX_LENGTH], int*);//removes empty lines
void splitEquations(char[], char[], char[]);                  //splits the expression into two distinct equations
List convertToList(char[]);                                   //converts an equation to a linked list for easy manipulation of data
void fixUnaryNumbers(List);                                   //allows for a number to be minus
void makeMinus(char[]);                                       //companion to the function above. makes node's data minus
void fixSigns(List);                                          //fixes equations where there is both - and +
bool deleteNodeAfterPosition(Position);                       //deletes a node from a list
List infixToPostfix(List);                                    //returns the postfix of an equation using stack
void singleCharToString(char, char[]);                        //puts a single character in a string of 2 so it can be added to the stack
bool isOperator(char[]);                                      //checks if element is an operator
bool isOperand(char[]);                                       //checks if an element is a number regardless of the sign
bool isOpening(char[]);                                       //checks if an element is a brace opening
bool isClosing(char[]);                                       //same as above but for closing braces
bool isPlus(char[]);                                          //self explanatory
bool isMinus(char[]);
bool hasHigherPriority(char[], char[]);                       //checks if an operator has a higher priority than another
int getPriority(char[]);                                      //returns an int value for easy comparing of priorities
double evaluatePostfix(List, bool*);                                 //gets the value of an equation in postfix format
void doubleToString(double, char[]);                          //turns a double value to a string
double calculate(double, char[], double, bool*);                     //calculates the answer of a simple equation of 2 numbers and an operator
bool checkBraceBalance(List);                                 //checks the balance of the equation using stack
bool isMatching(char[], char[]);                              //checks if two open-close braces are of the same type
List removeAllBraces(List);                                   //self explanatory
bool checkSyntaxError(List);                                  //checks all other errers in an equation
void printData(Stack S);                                      //prints the data in a list or stack
bool isValid(List L);                                         //all validity checks in one function
void showMenu();                                              //prints the menu to the user
void printAllEquations(char[][EQUATION_LENGTH], char[][EQUATION_LENGTH], int);//prints all equations given from file to the user
void printAllValidTrueExpressions(List[][2], int);             //prints expressions that have two valid equations and if they are equal
void printInvalidExpressions(List[][2], int, char[][EQUATION_LENGTH], char[][EQUATION_LENGTH]);//prints invalid expressions and if one equation is valid it will show the answer to it
void printValidityToFile(List[][2], int, char[][EQUATION_LENGTH], char[][EQUATION_LENGTH]);    //prints each equation and if its valid of not and its postfix if it is on a file
void openMenu();                                               //starts the program for the user to interact with

int main() {
    openMenu();
    return 0;
}
 
bool isEmpty(Stack S){
    return (S->next == NULL);
}

Stack createHead(){
    Stack S;
    S = (Stack) malloc(sizeof(node_t));
    if(S == NULL){
        printf("OUT OF RAM! malloc related error");
        return NULL;
    }
    S->next = NULL;
    return S;
}

StackPointer newNode(char Gdata[]){  //G stands for Given
    StackPointer new = (StackPointer) malloc(sizeof(node_t));
    if(new == NULL){
        printf("OUT OF RAM! malloc related error");
        return NULL;
    }
    strcpy(new->data, Gdata);
    new->next = NULL;
    return new;
}

void push(Stack S, char Gdata[]){
    StackPointer new = newNode(Gdata);
    new->next = S->next;
    S->next = new;
}

void top(Stack S, char string[]) {
    if(isEmpty(S)){
        strcpy(string, "_$_");         //_$_ is a indicator of an empty stack
        return;
    }
    strcpy(string, S->next->data);
}

void pop(Stack S){
    if(isEmpty(S)) {
        return;
    }

    StackPointer firstElement = S->next;
    S->next = S->next->next;
    free(firstElement);
}

int fileLineCount(FILE *file){
    int count = 0;
    char ch;
    do{
        ch = (char) fgetc(file);
        if(ch == '\n')
            count++;
    } while (!feof(file));
    rewind(file);     //returns the cursor to the beginning of the file for future use
    return count+1;   //+1 because it starts from 0
}

void getFromFile(FILE *file, char ret[][SCANNED_STRING_MAX_LENGTH]){
    int i = 0;
    char buffer[SCANNED_STRING_MAX_LENGTH];
    while (fgets(buffer, SCANNED_STRING_MAX_LENGTH, file) != NULL) {
        removeNewLine(buffer);
        strcpy(ret[i++], buffer);
    }
    fclose(file);
}

void removeSpaces(char s[]){
    int i = 0, k = 0;
    for(i = 0; i < strlen(s); i++){
        if(s[i] == ' '){
            for(k = i; k < strlen(s); k++){  //shifting all characters so we have a tidy string when space is removed
                s[k] = s[k+1];
            }
            s[k-1] = '\0';   //setting last element to null so it declares the end of the list
            i--;
        }
    }
}

void removeNewLine(char s[]){   //same as removeSpaces
    int i = 0, k = 0;
    for(i = 0; i < strlen(s); i++){
        if(s[i] == '\n'){
            for(k = i; k < strlen(s); k++){
                s[k] = s[k+1];
            }
            s[k-1] = '\0';
        }
    }
}

void removeEmptylines(char allEquations[][SCANNED_STRING_MAX_LENGTH], int *numberOfLines){
    int i = 0, k = 0;
    for (i = 0; i < *numberOfLines; i++){
        removeNewLine(allEquations[i]);
        if( strcmp(allEquations[i], "") == 0 ){
            for (k = i; k < *numberOfLines; k++){
                strcpy(allEquations[k], allEquations[k+1]);
            }
            *numberOfLines = *numberOfLines - 1;
            i--;
        }
    }
}

void splitEquations(char OGEquation[], char firstEuqation[], char secondEquation[]){ //OG stands for Original Gangster
    removeSpaces(OGEquation);
    char *token;


    if(OGEquation[0] == '='){
        strcpy(firstEuqation, " ");
        token = strtok(OGEquation, "=");
        strcpy(secondEquation, token);
    }
    else if(OGEquation[strlen(OGEquation) - 1] == '='){
        strcpy(secondEquation, " ");
        strncat(firstEuqation, OGEquation, strlen(OGEquation) - 1);
    }

    else {
        token = strtok(OGEquation, "=");     //splits all equations seperated by = into two
        strcpy(firstEuqation, token);
        token = strtok(NULL, "=");
        strcpy(secondEquation, token);
    }
}

bool insertLast(List L, char name[]){
    Position p = L, new = newNode(name);
    if(new == NULL) {
        printf("OUT OF RAM! malloc related error");
        return false;
    }
    while(p->next != NULL)
        p = p->next;
    p->next = new;
    new->next = NULL;
    return true;
}

List convertToList(char equation[]){
    List converted = createHead();
    char singleCharString[2];                 //to store each character for easy concatination to a string
    char tempString[DATA_STRING_LENGTH] = ""; //to help with numbers that have multiple digits

    int i = 0;
    while (i < strlen(equation)){             //using a while here because its easier to deal with multi digit numbers
       singleCharToString(equation[i], singleCharString);

        if(isOperand(singleCharString)){
            strcpy(tempString, "");
            while (isOperand(singleCharString)){
                strcat(tempString, singleCharString);
                i++;
                singleCharToString(equation[i], singleCharString);
            }
            insertLast(converted, tempString);
        }

       else {
            insertLast(converted, singleCharString);
            i++;
        }

    }

    return converted;
}

void fixUnaryNumbers(List L){

    Position p = L->next;
    Position first = L;

    if(first->next != NULL && first->next->next != NULL) {
        if (isMinus(first->next->data)) {             //if the first character was minus insert it to the number after it
            if (isOperand(first->next->next->data)) {
                makeMinus(first->next->next->data);
                deleteNodeAfterPosition(first);
            }
        } else if (isPlus(first->next->data)) {
            if (isOperand(first->next->next->data))
                deleteNodeAfterPosition(first);
        }
    }

    while (p != NULL && p->next != NULL && p->next->next != NULL) {   //doesnt need to check p if the next and the next next are not there

        if (strcmp(p->data, "*") == 0 || strcmp(p->data, "/") == 0 || strcmp(p->data, "^") == 0 || strcmp(p->data, "%") == 0) {  //so *- would work

            if (isMinus(p->next->data)) {
                if (isOperand(p->next->next->data)) {
                    makeMinus(p->next->next->data);
                    deleteNodeAfterPosition(p);
                }
            }

            else if (isPlus(p->next->data)) {
                if (isOperand(p->next->next->data)) {
                    deleteNodeAfterPosition(p);
                 }
            }
        }

        else if (isOpening(p->data) && isMinus(p->next->data) && isOperand(p->next->next->data)) {
            makeMinus(p->next->next->data);
            deleteNodeAfterPosition(p);
        }

        else if (isOpening(p->data) && isPlus(p->next->data) && isOperand(p->next->next->data)) {
            deleteNodeAfterPosition(p);
        }

        p = p->next;
    }
}

void makeMinus(char c[]){
    int number = atoi(c);           //converting string to int making it minus then back to string
    sprintf(c, "%d", number * -1);
}

void fixSigns(List L){
    Position p = L->next;
    while (p != NULL && p->next != NULL){

        if(isMinus(p->data) || isPlus(p->data)) {

            while (isPlus(p->next->data) || isMinus(p->next->data)){     //because the list changes inside this loop
                if (isMinus(p->data) && isMinus(p->next->data)){         //-- will become +
                    strcpy(p->data, "+");
                    deleteNodeAfterPosition(p);
                }

               else if(isMinus(p->data) && isPlus(p->next->data)){      //-+ will remove the plus
                    deleteNodeAfterPosition(p);
                }

                else if (isPlus(p->data) && isMinus(p->next->data)){   //+- same as above
                    strcpy(p->data, "-");
                    deleteNodeAfterPosition(p);
                }

                else if(isPlus(p->data) && isPlus(p->next->data)){     //++ will remove one of them
                    deleteNodeAfterPosition(p);
                }


            }
        }

        p = p->next;
    }
}

bool deleteNodeAfterPosition(Position p) {
    Position temp;
    if(p->next == NULL){
        printf("Nothing To Delete!");
        return false;
    }
    temp = p->next;
    p->next = temp->next;
    free(temp);
    return true;
}

List infixToPostfix(List infix){
    List postfix = createHead();
    Stack S = createHead();
    Position p = infix->next;
    char topTemp[DATA_STRING_LENGTH] = "";

    while (p != NULL){

        if(isOperand(p->data))
            insertLast(postfix, p->data);

        else if(isOperator(p->data)){

            top(S, topTemp);
            while (!isEmpty(S) && !isOpening(topTemp) && hasHigherPriority(topTemp, p->data)){    //the stack always has to have the operators stacked woth the most priority up
                insertLast(postfix, topTemp);
                pop(S);
                top(S, topTemp);
            }
            push(S, p->data);
        }

        else if(isOpening(p->data))
            push(S, p->data);

        else if (isClosing(p->data)){

            top(S, topTemp);
            while (!isEmpty(S) && !isOpening(topTemp)){
                insertLast(postfix, topTemp);
                pop(S);
                top(S, topTemp);
            }
            pop(S);     //popping one last time to get rid of the open brace
        }

        p = p->next;
    }

    while (!isEmpty(S)){         //adding everything left inthe stack to the postfix list
        top(S, topTemp);
        insertLast(postfix, topTemp);
        pop(S);
    }
    return postfix;
}

void singleCharToString(char input, char output[]){
    if (input == '\0'){
        strcpy(output, "$");   //$ is an identifier for empty char. helps when converting the last number to list
        return;
    }
    output[0] = input;
    output[1] = '\0';
}

bool isOperator(char op[]){
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "^") == 0 || strcmp(op, "%") == 0)
        return true;
    else
        return false;
}

bool isOperand(char c[]){
    int i = 0;
    bool flag = true;

    if(isOperator(c))
        return false;

    else if(c[0] == '-') {                       //to make it work with minus numbers
        for (i = 1; i < strlen(c); i++) {

            if (!(c[i] >= '0' && c[i] <= '9'))
                flag = false;
        }
        return flag;
    }
    else{
        for (i = 0; i < strlen(c); i++) {

            if (!(c[i] >= '0' && c[i] <= '9'))
                flag = false;
        }
        return flag;
    }
}

bool isOpening(char c[]){
    if (strcmp(c, "(") == 0 || strcmp(c, "{") == 0 || strcmp(c, "[") == 0)
        return true;
    else
        return false;
}

bool isClosing(char c[]){
    if (strcmp(c, ")") == 0 || strcmp(c, "}") == 0 || strcmp(c, "]") == 0)
        return true;
    else
        return false;
}

bool isPlus(char c[]){
    if(strcmp(c, "+") == 0)
        return true;
    else
        return false;
}

bool isMinus(char c[]){
    if(strcmp(c, "-") == 0)
        return true;
    else
        return false;
}

bool hasHigherPriority(char op1[], char op2[]){
    if ( getPriority(op1) >= getPriority(op2))
        return true;
    else
        return false;
}

int getPriority(char op[]){
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0)
        return 1;

    else if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "%") == 0)
        return 2;

    else if (strcmp(op, "^") == 0)
        return 3;

    else
        return -1;
}

double evaluatePostfix(List L, bool *divisionByZeroFlag){
    *divisionByZeroFlag = false;
    Stack S = createHead();
    Position p = L->next;
    double op1, op2, result;
    char topTemp[DATA_STRING_LENGTH];
    char stringTemp[DATA_STRING_LENGTH];

    while (p != NULL){

        if (isOperand(p->data))
            push(S, p->data);

        else{
            top(S, topTemp);           //the first numer is stored below the second. thats why we need to pop the second first
            op2 = atof(topTemp);
            pop(S);

            top(S, topTemp);
            op1 = atof(topTemp);
            pop(S);

            result = calculate(op1, p->data, op2, divisionByZeroFlag);
            doubleToString(result, stringTemp);
            push(S, stringTemp);
        }

        p = p->next;
    }
    top(S, topTemp);
    return (atof(topTemp));
}

void doubleToString(double number, char outputString[]){
    sprintf(outputString, "%f", number);
}

double calculate(double op1, char operation[], double op2, bool *divisionByZeroFlag){
    double answer;
    char firstChar = operation[0];
    switch (firstChar){
        case '+':
            answer = op1 + op2;
            break;

        case '-':
            answer = op1 - op2;
            break;

        case '*':
            answer = op1 * op2;
            break;

        case '/':
            if(op2 == 0){
                *divisionByZeroFlag = true;
                answer = op1 / op2;
            }
            else
                answer = op1 / op2;
            break;

        case '^':
            answer = pow(op1, op2);
            break;

        case '%':
            if(op2 == 0){
                *divisionByZeroFlag = true;
                answer = fmod(op1, op2);
            }
            else
                answer = fmod(op1, op2);
            break;

        default:
            printf("not an operation!\n");     //to identify when a problem has happned
            answer = 0.0;
    }
    return answer;
}

bool checkBraceBalance(List L){
    Stack S = createHead();
    Position p = L->next;
    char braceTemp[2];

    while (p != NULL){

        if (isOpening(p->data))
            push(S, p->data);

        else if (isClosing(p->data)) {


            top(S, braceTemp); //only tops when its a closing so there is no way braceTemp could over fill
            pop(S);

            if (strcmp(braceTemp, "_$_") == 0)
                return false;

            else if (!isMatching(braceTemp, p->data))
                return false;

        }

        p = p->next;
    }

    if (isEmpty(S))
        return true;
    else
        return false;

}

bool isMatching(char open[], char close[]){
    //printf("Tring to match %s and %s\n", open, close);
    if( (strcmp(open, "(") == 0 && strcmp(close, ")") == 0  || strcmp(open, "{") == 0 && strcmp(close, "}") == 0  ||  strcmp(open, "[") == 0 && strcmp(close, "]") == 0 ) )
        return true;
    else
        return false;
}

List removeAllBraces(List L){
    List skimmed = createHead();
    Position p = L;
    while (p->next != NULL){

        if(!isOpening(p->next->data) && !isClosing(p->next->data))
            insertLast(skimmed, p->next->data);

        p = p->next;
    }
    return skimmed;
}

bool checkSyntaxError(List withBraces){

    /*first part to test errors related to braces*/
    Position pb = withBraces->next;  //pb = positions with bares
    while (pb != NULL && pb->next != NULL){

        if(isOpening(pb->data) && isOperator(pb->next->data))
            return false;

        else if(!isOperand(pb->data) && !isOperator(pb->data) && !isOpening(pb->data) && !isClosing(pb->data))
            return false;

        pb = pb->next;
    }

    /*second part to test errors related to numbers and operators*/
    List withoutBraces = removeAllBraces(withBraces);
    Position p = withoutBraces->next;

    if(strcmp(p->data, " ") == 0)
        return false;

    if(p != NULL) {
        if (isOperator(p->data))
            return false;
    }

    while (p != NULL){

        if(isOperand(p->data)){
            if(p->next == NULL)
                return true;

            else if(isOperand(p->next->data))   //if two operands are next to each other its wrong
                return false;
        }

        else if(isOperator(p->data)){
            if(p->next == NULL)
                return false;

            else if (isOperator(p->next->data))     //if twi operators are next to each other its wrong
                return false;
        }

        p = p->next;
    }
    return true;
}

bool isValid(List L){                                  //does al the validity checks at once
    if(checkBraceBalance(L) && checkSyntaxError(L))
        return true;
    else
        return false;
}

void printData(Stack S){
    StackPointer p = S->next;
    while (p != NULL){
        printf("%s ", p->data);
        p = p->next;
    }
    //printf("\n");
}

void showMenu(){
    printf("\n----------------------------------------------------------------------\n");
    printf("Choose one of the following options by typing its corresponding number:\n");
    printf("-----------------------------------------------------------------------\n");
    printf("1. Display all equations from file.\n2. Display all Valid equations and if the expression is true.\n"
                   "3. Display equations from invalid expressions.\n4. Get an output file with input equations and their postfix.\n"
                   "-1. Exit!\n");
    printf("------------------------------------------------------------------------\n\n");
}

void printAllEquations(char first[][EQUATION_LENGTH], char second[][EQUATION_LENGTH], int numberOfEquations){
    int i = 0;
    for (i = 0; i < numberOfEquations; i++){
        printf("%s = %s\n", first[i], second[i]); //instead of puts because of space
    }
}

void printAllValidTrueExpressions(List equations[][2], int numberOfLines){

    bool divisionByZeroFlag;
    int i = 0;
    printf("-------------------------------------------------------------------------------\n");
    for (i = 0; i < numberOfLines; i++){

        if(isValid(equations[i][0]) && isValid(equations[i][1])){
            double firstEvaluated, secondEvaluated;
            List firstPostfix, secondPostfix;

            firstPostfix = infixToPostfix(equations[i][0]);
            secondPostfix = infixToPostfix(equations[i][1]);

            firstEvaluated = evaluatePostfix(firstPostfix, &divisionByZeroFlag);
            if(divisionByZeroFlag)
                continue;

            secondEvaluated = evaluatePostfix(secondPostfix, &divisionByZeroFlag);
            if(divisionByZeroFlag)
                continue;

            printf("[");
            printData(firstPostfix);
            printf("= %0.2f] =? [", firstEvaluated);
            printData(secondPostfix);
            printf("= %0.2f]", secondEvaluated);

            if(firstEvaluated == secondEvaluated)
                printf(" -> True\n");
            else
                printf(" -> False\n");
            printf("-------------------------------------------------------------------------------\n");
        }
    }
}

void printInvalidExpressions(List equations[][2], int numberOfLines, char firstEquation[][EQUATION_LENGTH], char secondEquation[][EQUATION_LENGTH]){

    bool divisionByZeroFlag = false;
    int i = 0;
    printf("-------------------------------------------------------------------------------\n");
    for (i = 0; i < numberOfLines; i++){

        if(!isValid(equations[i][0]) || !isValid(equations[i][1])){

            if(isValid(equations[i][0])){
                double firstEvaluated;
                List firstPostfix = infixToPostfix(equations[i][0]);
                firstEvaluated = evaluatePostfix(firstPostfix, &divisionByZeroFlag);
                if(!divisionByZeroFlag)
                    printf("%s -> valid -> %0.2f\n", firstEquation[i], firstEvaluated);
            } else if(!isValid(equations[i][0]))
                printf("%s -> invalid\n", firstEquation[i]);

            if (isValid(equations[i][1])){
                double secondEvaluated;
                List secondPostfix = infixToPostfix(equations[i][1]);
                secondEvaluated = evaluatePostfix(secondPostfix, &divisionByZeroFlag);
                if(!divisionByZeroFlag)
                    printf("%s -> valid -> %0.2f\n", secondEquation[i], secondEvaluated);
            } else if(!isValid(equations[i][1]))
                printf("%s -> invalid\n", secondEquation[i]);

            printf("-------------------------------------------------------------------------------\n");
        }

        else if(isValid(equations[i][0]) && isValid(equations[i][1])){
            bool secondDBZF = false; //second division by zero flag
            double firstEvaluated, secondEvaluated;
            List firstPostfix, secondPostfix;

            firstPostfix = infixToPostfix(equations[i][0]);
            secondPostfix = infixToPostfix(equations[i][1]);

            firstEvaluated = evaluatePostfix(firstPostfix, &divisionByZeroFlag);
            secondEvaluated = evaluatePostfix(secondPostfix, &secondDBZF);

            if(divisionByZeroFlag || secondDBZF){

                if(divisionByZeroFlag)
                    printf("%s -> invalid\n", firstEquation[i]);
                else
                    printf("%s -> valid -> %0.2f\n", firstEquation[i], firstEvaluated);


                if(secondDBZF)
                    printf("%s -> invalid\n", secondEquation[i]);
                else
                    printf("%s -> valid -> %0.2f\n", secondEquation[i], secondEvaluated);

                printf("-------------------------------------------------------------------------------\n");
            }

        }
    }
}

void printValidityToFile(List equations[][2], int numberOfLines, char firstEquation[][EQUATION_LENGTH], char secondEquation[][EQUATION_LENGTH]){

    printf("Where would you like to store the output file? (please state the name of the file as well):\n");
    char fileName[FILE_NAME_LENGTH];
    scanf("%s", fileName);
    FILE *outputFile;
    outputFile = fopen(fileName, "w");

    bool divisionByZeroFlag = false;
    int i = 0;
    for (i = 0; i < numberOfLines; i++){

        fprintf(outputFile, "---------------------------------------------------------------------------------\n");
        if(isValid(equations[i][0])){
            List firstPostfix = infixToPostfix(equations[i][0]);
            fprintf(outputFile, "%s -> ", firstEquation[i]);
            double firstEvaluated;
            firstEvaluated = evaluatePostfix(firstPostfix, &divisionByZeroFlag);
            if (!divisionByZeroFlag) {
                fprintf(outputFile, "valid -> ");
                Position p = firstPostfix->next;
                while (p != NULL) {
                    fprintf(outputFile, "%s ", p->data);
                    p = p->next;
                }
                fprintf(outputFile, " -> %0.2f\n", firstEvaluated);
            }
            if(divisionByZeroFlag)
                fprintf(outputFile, "invalid\n");
        } else
            fprintf(outputFile, "%s -> invalid\n", firstEquation[i]);

        if (isValid(equations[i][1])){
            List secondPostfix = infixToPostfix(equations[i][1]);
            fprintf(outputFile, "%s -> ", secondEquation[i]);
            double secondEvaluated;
            secondEvaluated = evaluatePostfix(secondPostfix, &divisionByZeroFlag);
            if(!divisionByZeroFlag) {
                fprintf(outputFile, "valid -> ");
                Position p = secondPostfix->next;
                while (p != NULL) {
                    fprintf(outputFile, "%s ", p->data);
                    p = p->next;
                }
                fprintf(outputFile, " -> %0.2f\n", secondEvaluated);
            }
            if(divisionByZeroFlag)
                fprintf(outputFile, "invalid\n");
        } else
            fprintf(outputFile, "%s -> invalid\n", secondEquation[i]);
    }
    fprintf(outputFile, "---------------------------------------------------------------------------------\n");
    fclose(outputFile);
    printf("All Done! go check %s file!\n", fileName);
}

void openMenu(){

    FILE *equationsFile;

    //equationsFile = fopen("/Users/AhmadSawi/Desktop/Data Structures/Project2/Equations.txt", "r");
    char equationsFileName[FILE_NAME_LENGTH];
    printf("enter the name and path of the file that has the equations in it:\n");
    scanf("%s", equationsFileName);
    equationsFile = fopen(equationsFileName, "r");

    int numberOfLines = fileLineCount(equationsFile);

    char allEquations[numberOfLines][SCANNED_STRING_MAX_LENGTH];

    getFromFile(equationsFile, allEquations);
    removeEmptylines(allEquations, &numberOfLines);

    List equations[numberOfLines][2];
    int i = 0;
    for (i = 0; i < numberOfLines; i++) {
        equations[i][0] = createHead();
        equations[i][1] = createHead();
    }

    char first[numberOfLines][EQUATION_LENGTH], second[numberOfLines][EQUATION_LENGTH];
    for (i = 0; i < numberOfLines; i++) {
        splitEquations(allEquations[i], first[i], second[i]);

        List firstConverted = convertToList(first[i]);
        List secondConverted = convertToList(second[i]);

        fixSigns(firstConverted);
        fixSigns(secondConverted);

        fixUnaryNumbers(firstConverted);
        fixUnaryNumbers(secondConverted);

        equations[i][0] = firstConverted;
        equations[i][1] = secondConverted;
    }

    int menuCounter = 1;

    while (menuCounter != -1) {
        showMenu();
        scanf("%d", &menuCounter);
        switch (menuCounter){
            case 1:
                printAllEquations(first, second, numberOfLines);
                break;

            case 2:
                printAllValidTrueExpressions(equations, numberOfLines);
                break;

            case 3:
                printInvalidExpressions(equations, numberOfLines, first, second);
                break;

            case 4:
                printValidityToFile(equations, numberOfLines, first, second);
                break;

            case -1:
                printf("Thanks!\n");
                break;

            default:
                printf("wrong entry! please chooe one of the numbers above!\n");
        }
    }
}