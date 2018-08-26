#ifndef MAIN_H
#define MAIN_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

typedef struct queue
{
  int operation_type;
  int dest_reg_no;
  int src1_reg_no;
  int src2_reg_no;
  char mem_address[9];
  char pc_address[9];
  int instn_tag;
  bool completed;
  char instn_state[3];
  int IF_start_cycle;
  int ID_start_cycle;
  int IS_start_cycle;
  int EX_start_cycle;
  int WB_start_cycle;

  struct queue *next;
}fifo_node;

typedef struct dispatchList
{
  int instn_tag;
  char instn_state[3];

  struct dispatchList *next;
}dispatch_node;

typedef struct dispatchIssueList
{
  int instn_tag;

  struct dispatchIssueList *next;
}dispatch_issue_node;

typedef struct issueList
{
  int instn_tag;
  int src1_rdy_bit;
  int src2_rdy_bit;
  int src1_tag;
  int src2_tag;

  struct issueList *next;
}issue_node;

typedef struct issueExecuteList
{
  int instn_tag;

  struct issueExecuteList *next;
}issue_execute_node;

typedef struct executeList
{
  int instn_tag;
  int cycle;

  struct executeList *next;
}execute_node;

typedef struct rmt
{
  int instn_tag;
  int valid;
}rf;

void add2dispatch(int*, char*, int*);
void advance_state2ID(int*);
void move_from_dispatch(int*);
void del_from_dispatch(int*);
bool isDispatchFull(int*);
bool isDispatchEmpty();
void sort_dispatch_issue_agent();
int del_from_dispatch_issue_agent();
void clear_dispatch_issue_agent();
bool isDispatchIssueAgentEmpty();
void add2issue(int*, int*);
void move_from_issue(int*);
void advance_state2IS(int*, int*);
void del_from_issue(int*);
bool isIssueEmpty();
void sort_issue_execute_agent();
void clear_issue_execute_agent();
int del_from_issue_execute_agent();
void add2execute(int*, int*, int*, int*, int*, int*);
void advance_state2EX(int*, int*);
void execute(int*);
void del_from_execute();
bool isExecuteFull(int*);
bool isExecuteEmpty();
void enqueue(int*, int*, int*, int*, char*, char*, int*, int*);
bool modify_fifo_state(int*, char*);
void fake_retire();
fifo_node* dequeue();
void display();
bool advance_cycle();

#endif
