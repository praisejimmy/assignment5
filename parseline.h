#ifndef PARSELINE_H
#define PARSELINE_H

struct stage;
void set_stage(char *cmd, struct stage *stg, int stage);
void print_stages(struct stage *stages);

#endif
