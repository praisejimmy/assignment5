#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "parseline.h"

#define LINE_MAX 512
#define ARG_MAX 10
#define PIPE_MAX 10

/* contains all information pertaining to stage of a piped command */
struct stage {

    char *command;
    char input[18];
    char output[18];
    int argc;
    char *argv[10];

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
        strcpy(stg->input, "original stdin");
        if (stage_cnt > 1) {
            strcpy(stg->output, "pipe to stage 1");
        }
        else {
            strcpy(stg->output, "original stdout");
        }
    }
    else if (stage != 0) {
        sprintf(stg->input, "pipe from stage %d", stage - 1);
        if (stage == (stage_cnt - 1)) {
            strcpy(stg->output, "original stdout");
        }
        else {
            sprintf(stg->output, "pipe to stage %d", stage + 1);
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
            strcpy(stg->input, token);
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
            strcpy(stg->output, token);
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
        printf("     input: %s\n", stages[i].input);
        printf("    output: %s\n", stages[i].output);
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

