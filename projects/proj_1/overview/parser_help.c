
//COP4610
//Spring 2019
//Project 1 Starter Code
//example code for initial parsing

//*** if any problems are found with this code,
//*** please report them to the TA


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
	char** tokens;
	int numTokens;
} instruction;


void addToken(instruction* instr_ptr, char* tok);
void printTokens(instruction* instr_ptr);
void clearInstruction(instruction* instr_ptr);
 
int main() {
	char* token = NULL;
	char* temp = NULL;

	instruction instr;
	instr.tokens = NULL;
	instr.numTokens = 0;
	

    while (1) {
		printf("Please enter an instruction:");

        do {			// loop reads character sequences separated by whitespace
            scanf( "%ms", &token);									//scans for next token and allocates token var to size of scanned token
			temp = (char*)malloc((strlen(token)+1) * sizeof(char));	//allocate temp variable with same size as token
			
			int i;
			int start;
			
			start = 0;
			
			for (i = 0; i < strlen(token); i++)
			{
				//pull out special characters and make them into a separate token in the instruction
				if (token[i] == '|' || token[i] == '>' || token[i] == '<' || token[i] == '&') 
				{
					if (i-start > 0)
					{
						memcpy(temp, token + start, i - start);
						temp[i-start] = '\0';
						addToken(&instr, temp);						
					}
					
					char specialChar[2];
					specialChar[0] = token[i];
					specialChar[1] = '\0';
					
					addToken(&instr,specialChar);
			
					start = i + 1;
				}
			}
			if (start < strlen(token))
			{
				memcpy(temp, token + start, strlen(token) - start);
				temp[i-start] = '\0';
				addToken(&instr, temp);			
			}

			//free and reset variables
			free(token);
			free(temp);

			token = NULL;
			temp = NULL;
		
        } while ('\n' != getchar());    //until end of line is reached
		
		printTokens(&instr);
		clearInstruction(&instr);
		
    }

    return 0;
}

//reallocates instruction array to hold another token
//allocates for new token within instruction array
void addToken(instruction* instr_ptr, char* tok)
{
	//extend token array to accomodate an additional token
	if (instr_ptr->numTokens == 0)
		instr_ptr->tokens = (char**)malloc(sizeof(char*));
	else
		instr_ptr->tokens = (char**)realloc(instr_ptr->tokens, (instr_ptr->numTokens+1) * sizeof(char*));

	//allocate char array for new token in new slot
	instr_ptr->tokens[instr_ptr->numTokens] = (char *)malloc((strlen(tok)+1) * sizeof(char));
	strcpy(instr_ptr->tokens[instr_ptr->numTokens], tok);

	instr_ptr->numTokens++;
	
}

void printTokens(instruction* instr_ptr)
{
	int i;
	printf("Tokens:\n");
	for (i = 0; i < instr_ptr->numTokens; i++)
		printf("#%s#\n", (instr_ptr->tokens)[i]);
}

void clearInstruction(instruction* instr_ptr)
{
	int i;
	for (i = 0; i < instr_ptr->numTokens; i++)
		free(instr_ptr->tokens[i]);
	free(instr_ptr->tokens);

	instr_ptr->tokens = NULL;
	instr_ptr->numTokens = 0;
}