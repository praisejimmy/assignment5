#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "parseline.h"

#define LINE_MAX 512
#define ARG_MAX 10
#define PIPE_MAX 10
#define PATH_MAX 4096

/* contains all information pertaining to stage of a piped command
input or output as -1 is std in or out respectively,
input or output as -2 means specific file named
files[0] is input file files[1] is output file */
struct stage {

    char *command;
    int input;
    int output;
    int argc;
    char *argv[10];
    char files[2][PATH_MAX];

};

struct stage stages[PIPE_MAX];
int stage_cnt;

int main(int argc, char **argv) {
    char command[LINE_MAX];
    char *pipes[PIPE_MAX];
    char *token;
    int stage = 0;
    stage_cnt = 0;
    memset(stages, '\0', sizeof(stage) * PIPE_MAX);
    memset(command, '\0', LINE_MAX);
    printf("line: ");
    scanf("%512[^\n]", command);
    token = strtok(command, "|");
    while (token != NULL) {
        if (stage_cnt == PIPE_MAX) {
            fprintf(stderr, "pipeline too deep\n");
            exit(-1);
        }
        if (!strcmp(token, " ")) {
            fprintf(stderr, "invalid null command\n");
            exit(-1);
        }
        pipes[stage_cnt] = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(pipes[stage_cnt], token);
        stage_cnt++;
        token = strtok(NULL, "|");
    }
    while (stage < stage_cnt) {
        set_stage(pipes[stage], &stages[stage], stage);
        stage++;
    }
    print_stages(stages);
    free_stages(stages);
    free_pipes(pipes);
    return 0;
}

/* takes in a command string, empty stage, and current stage number and fills
   the given stage with all pertinant information, erroring if otherwise */
void set_stage(char *cmd, struct stage *stg, int stage) {
    char *token;
    stg->argc = 0;
    stg->command = strdup(cmd);
    token = strtok(cmd, " ");

    if (stage == 0) {
        stg->input = -1;
        if (stage_cnt > 1) {
            stg->output = 1;
        }
        else {
            stg->output = -1;
        }
    }
    else if (stage != 0) {
        stg->input = stage - 1;
        if (stage == (stage_cnt - 1)) {
            stg->output = -1;
        }
        else {
            stg->output = stage + 1;
        }
    }

    while (token != NULL) {
        if (!strcmp(token, "<")) {
            token = strtok(NULL, " ");
            if (stage > 0) {
                fprintf(stderr, "%s: ambiguous input\n", cmd);
                exit(-1);
            }
            if (token == NULL || !strcmp(token, "<") || !strcmp(token, ">")) {
                fprintf(stderr, "%s: bad input redirection\n", cmd);
                exit(-1);
            }
            stg->input = -2;
            strcpy(stg->files[0], token);
        }
        else if (!strcmp(token, ">")) {
            token = strtok(NULL, " ");
            if (stage_cnt > 1 && stage < stage_cnt - 1) {
                fprintf(stderr, "%s: ambiguous output\n", cmd);
                exit(-1);
            }
            if (token == NULL || !strcmp(token, "<") || !strcmp(token, ">")) {
                fprintf(stderr, "%s: bad output redirection\n", cmd);
                exit(-1);
            }
            stg->output = -2;
            strcpy(stg->files[1], token);
        }
        else if(!strncmp(token, "'", 1)) {
            char *command = malloc(sizeof(char) * (strlen(token)));
            strcpy(command, token+1);
            token = strtok(NULL, "'");
            command = realloc(command, sizeof(char) * (strlen(command) + strlen(token) + 2));
            strcat(command, " "); /* because strtok got rid of space */
            strcat(command, token);
            stg->argv[stg->argc] = malloc(sizeof(char) * (strlen(command) + 1));
            strcpy(stg->argv[stg->argc], command);
            stg->argc++;
            free(command);
        }
        else {
            if (stg->argc == 10) {
                fprintf(stderr, "%s: too many arguments\n", cmd);
                exit(-1);
            }
            stg->argv[stg->argc] = malloc(sizeof(char) * (strlen(token) + 1));
            strcpy(stg->argv[stg->argc], token);
            stg->argc++;
        }
        token = strtok(NULL, " ");
    }
}

/* Given the array of stages, prints out all information for each stage */
void print_stages(struct stage *stages) {
    int i;
    int j;
    for (i = 0; i < stage_cnt; i++) {
        printf("--------\n");
        printf("Stage %d: \"%s\"\n", i, stages[i].command);
        printf("--------\n");
        if (stages[i].input == -1) {
            printf("     input: original stdin\n");
        }
        else if (stages[i].input == -2) {
            printf("     input: %s\n", stages[i].files[0]);
        }
        else {
            printf("     input: pipe from stage %d\n", stages[i].input);
        }
        if (stages[i].output == -1) {
            printf("    output: original stdout\n");
        }
        else if (stages[i].output == -2) {
            printf("    output: %s\n", stages[i].files[1]);
        }
        else {
            printf("    output: pipe to stage %d\n", stages[i].output);
        }
        printf("      argc: %d\n", stages[i].argc);
        printf("      argv: ");
        for (j = 0; j < stages[i].argc; j++) {
            printf("\"%s\"", stages[i].argv[j]);
            if (j < stages[i].argc - 1) {
                printf(",");
            }
        }
        printf("\n\n");
    }
}

/* free all allocated memory in stages */
void free_stages(struct stage *stages){
	int i, j;
	for(i = 0; i < stage_cnt; i++){
		for(j = 0; j < stages[i].argc; j++){
			free(stages[i].argv[j]);
		}
		free(stages[i].command);
	}
	return;
}

/* free all commands */
void free_pipes(char *pipes[]){
	int i;
	for(i = 0; i < stage_cnt; i++){
		free(pipes[i]);
	}
	return;
}
