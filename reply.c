//in$ cc reply.c -ledit -o r
//mpc.c -ledit -lm -o 
//
#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#include "mpc.h"

#ifdef _WIN32
#include <string.h>

static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}
void add_history(char* unused) {}
#else
#include <editline/readline.h>
#endif

void currentTime(){

	time_t rawtime;
    struct tm *ptminfo;
 
    time(&rawtime);
    ptminfo = localtime(&rawtime);
    printf("(current: %02d-%02d-%02d %02d:%02d:%02d)\n",
            ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
            ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);

}

long eval_op(long x , char * op ,long y){
  if (strcmp(op,"+") == 0){return x + y ;}
  if (strcmp(op,"-") == 0){return x - y ;}
  if (strcmp(op,"*") == 0){return x * y ;}
  if (strcmp(op,"/") == 0){return x / y ;}
  return 0;
}

long eval(mpc_ast_t* t){

  if (strstr(t->tag,"number"))
  {
    return atol(t->contents);
  }

  char *op = t->children[1]->contents;
  long x = eval(t->children[2]);

  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;  
}

int main(int argc, char const *argv[])
{

  /* Create Some Parsers */
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* YLispy    = mpc_new("Ylispy");

  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT,
  "                                                     \
    number   : /-?[0-9]+([.]?\\d+)?/;                             \
    operator : '+' | '-' | '*' | '/' | '%';                  \
    expr     : <number> | '(' <operator> <expr>+ ')' ;  \
    Ylispy    : /^/ <operator> <expr>+ /$/ ;             \
  ",
  Number, Operator, Expr, YLispy);

	printf("YLispy Version 0.0.0.2  ");
	currentTime();
	puts("Press Ctrl+c to Exit\n");
	while(1){
		char *input = readline("YLispy>>>");
		add_history(input);
		
    mpc_result_t r;
    if (mpc_parse("<stdin>", input,YLispy, &r)) {
        /* On Success Print the AST */
        // mpc_ast_print(r.output);

        // mpc_ast_t* t = r.output;
        // printf("Tag: %s\n", a->tag);
        // printf("Contents: %s\n", a->contents);
        // printf("Number of children: %i\n", a->children_num);

        printf("%ld\n", eval(r.output));
        /* Get First Child */
        // mpc_ast_t* c0 = a->children[0];
        // printf("First Child Tag: %s\n", c0->tag);
        // printf("First Child Contents: %s\n", c0->contents);
        // printf("First Child Number of children: %i\n",c0->children_num);

        mpc_ast_delete(r.output);
    } else {
        /* Otherwise Print the Error */
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
    }

		free(input);
	}
  mpc_cleanup(4, Number, Operator, Expr, YLispy);
	return 0;
}
