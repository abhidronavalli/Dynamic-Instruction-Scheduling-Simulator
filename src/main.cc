#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include "main.h"
#include "cacheFactory.h"
#define MAX_FIFO_SIZE 1024

static int fifo_count = 0;
static int dispatch_count = 0;
static int dispatch_issue_count = 0;
static int issue_count = 0;
static int issue_execute_count = 0;
static int execute_count = 0;

fifo_node *fifo_front = NULL;
fifo_node *fifo_rear = NULL;
dispatch_node *dispatch_front = NULL;
dispatch_node *dispatch_rear = NULL;
dispatch_issue_node *dispatch_issue_front = NULL;
dispatch_issue_node *dispatch_issue_rear = NULL;
issue_node *issue_front = NULL;
issue_node *issue_rear = NULL;
issue_execute_node *issue_execute_front = NULL;
issue_execute_node *issue_execute_rear = NULL;
execute_node *execute_front = NULL;
execute_node *execute_rear = NULL;
rf register_file[128];


void add2dispatch(int *instn_tag, char *instn_state, int *dispatch_limit)
{
  if(dispatch_count != *dispatch_limit)
  {
    dispatch_node *temp;

    temp = (dispatch_node *)malloc(sizeof(dispatch_node));


    temp->instn_tag = *instn_tag;
    strcpy(temp->instn_state, instn_state);
    if(strcpy(temp->instn_state, instn_state) == NULL) printf("Failed to add\n");

    if(dispatch_front == NULL)
    {
      dispatch_front = temp;
      dispatch_rear = dispatch_front;
      dispatch_rear->next = NULL;
    }
    else
    {
      dispatch_rear->next = temp;
      dispatch_rear = temp;
      temp->next = NULL;
    }
    dispatch_count++;
  }
  else
  {
    //printf("Dispatch list full!\n");
    return;
  }
}

void advance_state2ID(int *cycle)
{
  //this function traverses the dispatch list and advances any IF state instn to ID
  //printf("in advnce routine\n");
  dispatch_node *dispatch_temp = dispatch_front;
  fifo_node *fifo_temp = fifo_front;

  while(dispatch_temp != NULL)
  {
    if(strcmp(dispatch_temp->instn_state, "IF") == 0)
    {
      strcpy(dispatch_temp->instn_state, "ID");
      do
      {
        if(fifo_temp->instn_tag == dispatch_temp->instn_tag)
        {
          strcpy(fifo_temp->instn_state, "ID");
          fifo_temp -> ID_start_cycle = *cycle;
          //printf("FIFO:New state of %s is %s\n", fifo_temp->pc_address, fifo_temp->instn_state);
          //printf("FIFO:ID start cycle: %d\n", fifo_temp -> ID_start_cycle);
        }
        fifo_temp = fifo_temp->next;
      }while(fifo_temp != fifo_front);
    }
    //printf("New state of %d is %s\n", dispatch_temp->instn_tag, dispatch_temp->instn_state);
    dispatch_temp = dispatch_temp->next;

  }

}

void move_from_dispatch(int *n_parameter)
{
  //void add2dispatch_issue_agent(int *tag);
  dispatch_node *dispatch_temp = dispatch_front;
  dispatch_issue_node *temp;
  int ID_instn_count = 0;
  int instn_tag = 0;
  if(dispatch_temp == NULL)
  {
    //printf("dispatch list empty\n");
    return;
  }
  while(dispatch_temp != NULL)
  {
    if(strcmp(dispatch_temp->instn_state, "ID") == 0)
    {
      instn_tag = dispatch_temp->instn_tag;
      //printf("ading tag %d to temp list\n", instn_tag);
      //printf("dispatch_issue_count: %d\n",dispatch_issue_count);
      if(dispatch_issue_count != *n_parameter)
      {

        temp = (dispatch_issue_node *)malloc(sizeof(dispatch_issue_node));
        if(temp == NULL)
        {
          printf("Insuffient memory\n");
          exit(0);
        }

        temp->instn_tag = instn_tag;

        if(dispatch_issue_front == NULL)
        {
          dispatch_issue_front = temp;
          dispatch_issue_rear = dispatch_issue_front;
          dispatch_issue_rear->next = NULL;
        }
        else
        {
          dispatch_issue_rear->next = temp;
          dispatch_issue_rear = temp;
          dispatch_issue_rear->next = NULL;
        }
        dispatch_issue_count++;

      }
      else
      {
        //printf("Dispatch-Issue Agent list full!\n");
        return;
      }


      ID_instn_count++;
    }
    dispatch_temp = dispatch_temp->next;
  }
}

void del_from_dispatch(int *instn_tag)
{
  int count = 0;
  dispatch_node *temp = dispatch_front;
  dispatch_node *deleted_node;
  if(dispatch_count > 0)
  {

    if((dispatch_front == dispatch_rear)&&(dispatch_rear == NULL))
    {
      //printf("Dispatch list already empty\n");
      return;
    }
    else
    {
      if(dispatch_front == dispatch_rear)
      {
        //printf("Dispatch list has been emptied\n");
        dispatch_front = NULL;
        dispatch_rear = NULL;
      }
      else
      {
        while(temp != NULL)
        {
          if(temp->instn_tag == *instn_tag)
          {
            break;
          }
          count++;
          temp = temp->next;
        }

        if(count == 0)
        {
          temp = dispatch_front;
          //printf("deleting tag %d from dispatch\n", temp->instn_tag);
          dispatch_front = dispatch_front->next;
          free(temp);
        }
        else
        {
          temp = dispatch_front;
          for(int i=0;i<count;i++)
          {
            temp = temp->next;
          }
          deleted_node = temp->next;
          temp->next = deleted_node->next;
          deleted_node->next = NULL;
          //printf("deleting tag %d from dispatch\n", deleted_node->instn_tag);
          free(deleted_node);
        }
      }
        if(dispatch_count > 0) dispatch_count--;
    }
  }
  else
  {
    //printf("Dispatch list is empty\n");
    return;
  }
}


bool isDispatchFull(int *dispatch_limit)
{
  if(dispatch_count == *dispatch_limit)
  {
    return true;
  }
  else
  {
    return false;
  }
}
bool isDispatchEmpty()
{
  if(dispatch_count <= 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}
/*
void add2dispatch_issue_agent(int *instn_tag)
{

  if(dispatch_issue_count != (dispatch_limit/2))
  {
    dispatch_issue_node *temp;
    //printf("dispatch_issue_count: %d\n",dispatch_issue_count);
    temp = (dispatch_issue_node *)malloc(sizeof(dispatch_issue_node));
    if(temp == NULL)
    {
      printf("Insuffient memory\n");
      exit(0);
    }

    temp->instn_tag = *instn_tag;

    if(dispatch_issue_front == NULL)
    {
      dispatch_issue_front = temp;
      dispatch_issue_rear = dispatch_issue_front;
      dispatch_issue_rear->next = NULL;
    }
    else
    {
      dispatch_issue_rear->next = temp;
      dispatch_issue_rear = temp;
      temp->next = NULL;
    }
    dispatch_issue_count++;

  }
  else
  {
    //printf("Dispatch-Issue Agent list full!\n");
    return;
  }

}
*/
void sort_dispatch_issue_agent()
{
  int a = 0;
  dispatch_issue_node *temp1 = dispatch_issue_front;
  dispatch_issue_node *temp2;
  if(temp1 == NULL)
  {
    //printf("Cannot sort an empty DI agent list\n");
    return;
  }

  for(temp1 = dispatch_issue_front; temp1 != NULL; temp1 = temp1->next)
    {
        for(temp2 = temp1->next; temp2 != NULL; temp2 = temp2->next)
        {
          if(temp2->instn_tag < temp1->instn_tag)
          {
            a = temp1->instn_tag;
            temp1->instn_tag = temp2->instn_tag;
            temp2->instn_tag = a;
          }
        }
    }
}

int del_from_dispatch_issue_agent()
{
  int tag = 0;
  //printf("dispatch_issue_count(del from DI agent): %d\n",dispatch_issue_count);
  if(dispatch_issue_count > 0)
  {
    dispatch_issue_node *temp = dispatch_issue_front;
    dispatch_issue_node *deleted_node;
    if((dispatch_issue_front == dispatch_issue_rear)&&(dispatch_issue_rear == NULL))
    {
      //printf("Dispatch-Issue agent list already empty\n");
      return -1;
    }
    else
    {
      if(dispatch_issue_front == NULL)
      {
        //printf("Dispatch-Issue agent list has been emptied\n");
        dispatch_issue_front = NULL;
        dispatch_issue_rear = NULL;
      }
      else
      {
        tag = temp->instn_tag;
        deleted_node = dispatch_issue_front;
        dispatch_issue_front = dispatch_issue_front->next;
        free(deleted_node);
        //the popped data is sent to main
      }
    }
    if(dispatch_issue_count > 0) dispatch_issue_count--;

    return tag;
  }
  else
  {
    //printf("Dispatch-Issue agent is empty\n");
    return -1;
  }
}

void clear_dispatch_issue_agent()
{
  dispatch_issue_node *temp =  dispatch_issue_front;
  while(dispatch_issue_front != NULL)
  {
      temp = dispatch_issue_front;
      dispatch_issue_front = dispatch_issue_front->next;
      free(temp);
  }
  dispatch_issue_count = 0;
}

bool isDispatchIssueAgentEmpty()
{
  //printf("dispatch_issue_count: %d\n",dispatch_issue_count);
  if(dispatch_issue_count <= 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}
void add2issue(int *instn_tag, int *issue_limit)
{
  int src1=-1, src2=-1, dest=-1;
  int tag=0;
  int src1_not_ready=0, src2_not_ready=0;
  //register_file register_file;
  if(issue_count < *issue_limit)
  {
    issue_node *issue_temp;
    fifo_node *fifo_temp = fifo_front;
    issue_temp = (issue_node *)malloc(sizeof(issue_node));
    if(issue_temp == NULL)
    {
      printf("Insuffient memory\n");
      exit(0);
    }
    issue_temp->instn_tag = *instn_tag;
    //printf("added tag %d to issue list\n", *instn_tag);
    do
    {
      if(fifo_temp->instn_tag == *instn_tag)
      {
        src1 = fifo_temp->src1_reg_no;
        src2 = fifo_temp->src2_reg_no;
        dest = fifo_temp->dest_reg_no;
        break;
      }
      fifo_temp = fifo_temp->next;
    }while(fifo_temp != fifo_front);
    fifo_temp = fifo_front;


    if(src1 == -1)
    {
      issue_temp->src1_rdy_bit = 1;
      issue_temp->src1_tag = 0;
    }
    else
    {
      //printf("Valid src1 reg detected\n");
      if(register_file[src1].valid == 1)
      {
        issue_temp->src1_rdy_bit = 1;
        issue_temp->src1_tag = 0;
      }
      else
      {
        //printf("RF[%d].valid not set\n", src1);
        tag = register_file[src1].instn_tag;
        do
        {
          if(fifo_temp->instn_tag == tag && fifo_temp->completed == true)
          {
            issue_temp->src1_rdy_bit = 1;
            issue_temp->src1_tag = 0;
            break;
          }
          else src1_not_ready = 1;
          //printf("tag of current fifo: %d\n", fifo_temp->instn_tag);
          fifo_temp = fifo_temp->next;
        }while(fifo_temp != fifo_front);
        if(src1_not_ready == 1)
        {
          issue_temp->src1_rdy_bit = 0;
          issue_temp->src1_tag = tag;
        }
      }
    }


    if(src2 == -1)
    {
      issue_temp->src2_rdy_bit = 1;
      issue_temp->src2_tag = 0;
    }
    else
    {
      //printf("Valid src2 reg detected\n");
      if(register_file[src2].valid == 1)
      {
        issue_temp->src2_rdy_bit = 1;
        issue_temp->src2_tag = 0;
      }
      else
      {
        tag = register_file[src2].instn_tag;
        do
        {
          //printf("RF[%d].valid not set\n", src2);
          if(fifo_temp->instn_tag == tag && fifo_temp->completed == true)
          {
            issue_temp->src2_rdy_bit = 1;
            issue_temp->src2_tag = 0;
            break;
          }
          else src2_not_ready = 1;

          fifo_temp = fifo_temp->next;
        }while(fifo_temp != fifo_front);
        if(src2_not_ready == 1)
        {
          issue_temp->src2_rdy_bit = 0;
          issue_temp->src2_tag = tag;
        }
      }
    }

    if(dest >= 0)
    {
      //printf("dest no is: %d", dest);
      register_file[dest].instn_tag = *instn_tag;
      register_file[dest].valid = 0;
    }


    if(issue_front == NULL)
    {
      issue_front = issue_temp;
      issue_rear = issue_front;
      issue_rear->next = NULL;
    }
    else
    {
      issue_rear->next = issue_temp;
      issue_rear = issue_temp;
      issue_temp->next = NULL;
    }
    issue_count++;
  }
  else
  {
    //printf("Issue list full\n");
    return;
  }
}

void move_from_issue(int *n_parameter)
{
  issue_node *issue_temp = issue_front;
  fifo_node *fifo_temp = fifo_front;
  issue_execute_node *issue_execute_temp;
  int instn_tag = 0;
  while(issue_temp != NULL)
  {
    if(issue_temp->src1_rdy_bit == 1 && issue_temp->src2_rdy_bit == 1)
    {
      instn_tag = issue_temp->instn_tag;
      if(issue_execute_count != *n_parameter)//because only n FU so might as well sort n instn
      {
        issue_execute_temp = (issue_execute_node *)malloc(sizeof(issue_execute_node));
        if(issue_execute_temp == NULL)
        {
          printf("Insuffient memory\n");
          exit(0);
        }
        issue_execute_temp->instn_tag = instn_tag;

        if(issue_execute_front == NULL)
        {
          issue_execute_front = issue_execute_temp;
          issue_execute_rear = issue_execute_front;
          issue_execute_rear->next = NULL;
        }
        else
        {
          issue_execute_rear->next = issue_execute_temp;
          issue_execute_rear = issue_execute_temp;
          issue_execute_rear->next = NULL;
        }
        issue_execute_count++;
      }
      else
      {
        //printf("Issue-Execute Agent is full\n");
        return;
      }
    }
    issue_temp = issue_temp->next;
  }
}

void advance_state2IS(int *instn_tag, int *cycle)
{
  //printf("in advnce to IS routine\n");
  issue_node *issue_temp = issue_front;
  fifo_node *fifo_temp = fifo_front;

  while(issue_temp != NULL)
  {
    //printf("inside the advance to IS loop\n");
    if(issue_temp->instn_tag == *instn_tag)
    {
      do
      {
        if(fifo_temp->instn_tag == issue_temp->instn_tag)
        {
          strcpy(fifo_temp->instn_state, "IS");
          fifo_temp -> IS_start_cycle = *cycle;
          //printf("FIFO:New state of %s is %s\n", fifo_temp->pc_address, fifo_temp->instn_state);
          //printf("FIFO:IS start cycle: %d\n", fifo_temp -> IS_start_cycle);
        }
        fifo_temp = fifo_temp->next;
      }while(fifo_temp != fifo_front);
    }
    issue_temp = issue_temp->next;
  }
}

void del_from_issue(int *instn_tag)
{
  int count = 0;
  //if(isDispatchEmpty() == false)
  issue_node *temp = issue_front;
  issue_node *deleted_node;
  issue_node *prev;
  if(issue_count > 0)
  {

    if((issue_front == issue_rear)&&(issue_rear == NULL))
    {
      //printf("Issue list already empty\n");
      return;
    }
    else
    {
      if(issue_front == issue_rear)
      {
        //printf("Issue list has been emptied\n");
        issue_front = NULL;
        issue_rear = NULL;
        if(issue_count>0) issue_count--;
      }
      else
      {
        if(temp != NULL && temp->instn_tag == *instn_tag)
        {
          issue_front = temp->next;
          //printf("deleting tag %d from issue\n", temp->instn_tag);
          free(temp);
          temp = issue_front;
          issue_count--;
          return;
        }
        else
        {
          while(temp != NULL && temp->instn_tag != *instn_tag)
          {
            prev = temp;
            temp = temp->next;
          }

          if(temp == NULL)
          {
            printf("WARNING: Tag %d not present in issue list\n", *instn_tag);
          }

          prev->next = temp->next;
          if(temp->next == NULL)
          {
            issue_rear = prev;
            //printf("issue_rear is updated\n");
          }
          //temp->next = NULL;
          //printf("deleting tag %d from issue\n", temp->instn_tag);
          free(temp);
          issue_count--;
          return;
        }

        /*
        while(temp != NULL)
        {
          if(temp->instn_tag == *instn_tag)
          {
            break;
          }
          count++;
          temp = temp->next;
        }
        printf("count(del_from_issue): %d\n", count);
        if(count == 0)
        {
          temp = issue_front;
          printf("deleting tag %d from issue\n", temp->instn_tag);
          issue_front = issue_front->next;
          free(temp);
        }
        else if(count == 1)
        {
          deleted_node = issue_front->next;
          issue_front->next = deleted_node->next;
          deleted_node->next = NULL;
          printf("deleting tag %d from issue\n", deleted_node->instn_tag);
          free(deleted_node);
        }
        else
        {
          temp = issue_front;
          for(int i=0;i<count-1;i++)
          {
            temp = temp->next;
          }
          deleted_node = temp->next;
          temp->next = deleted_node->next;
          if(deleted_node->next == NULL) issue_rear = temp;
          deleted_node->next = NULL;
          printf("deleting tag %d from issue\n", deleted_node->instn_tag);
          free(deleted_node);
        }
        */
      }
        //if(issue_count > 0) issue_count--;
    }
  }
  else
  {
    //printf("Issue list is empty\n");
    return;
  }
}
/*
bool isIssueFull()
{
  if(issue_count == issue_limit)
  {
    return true;
  }
  else
  {
    return false;
  }
}
*/
bool isIssueEmpty()
{
  if(issue_count <= 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void sort_issue_execute_agent()
{
  int a = 0;
  issue_execute_node *temp1 = issue_execute_front;
  issue_execute_node *temp2;
  if(temp1 == NULL)
  {
    //printf("Cannot sort an empty IE agent list\n");
    return;
  }

  for(temp1 = issue_execute_front; temp1 != NULL; temp1 = temp1->next)
    {
        for(temp2 = temp1->next; temp2 != NULL; temp2 = temp2->next)
        {
          if(temp2->instn_tag < temp1->instn_tag)
          {
            a = temp1->instn_tag;
            temp1->instn_tag = temp2->instn_tag;
            temp2->instn_tag = a;
          }
        }
    }
}

void clear_issue_execute_agent()
{
  issue_execute_node *temp =  issue_execute_front;
  while(issue_execute_front != NULL)
  {
      temp = issue_execute_front;
      issue_execute_front = issue_execute_front->next;
      free(temp);
  }
  issue_execute_count = 0;
  //printf("Cleared IE agent list\n");
}

int del_from_issue_execute_agent()
{
  int tag = 0;
  //printf("issue_execute_count(del from IE agent): %d\n",issue_execute_count);
  if(issue_execute_count > 0)
  {
    issue_execute_node *temp = issue_execute_front;
    issue_execute_node *deleted_node;
    if((issue_execute_front == issue_execute_rear)&&(issue_execute_rear == NULL))
    {
      //printf("Issue-Execute agent list already empty\n");
      return -1;
    }
    else
    {
      if(issue_execute_front == NULL)
      {
        //printf("Issue-Execute agent list has been emptied\n");
        dispatch_issue_front = NULL;
        dispatch_issue_rear = NULL;
      }
      else
      {
        tag = temp->instn_tag;
        deleted_node = issue_execute_front;
        issue_execute_front = issue_execute_front->next;
        free(deleted_node);
        //the popped data is sent to main
      }
    }
    if(issue_execute_count > 0) issue_execute_count--;

    return tag;
  }
  else
  {
    //printf("Issue-Execute agent is empty\n");
    return -1;
  }

}

void add2execute(int *instn_tag, int *blk_size, int *l1_cache_size, int *l1_associativity, int *l2_cache_size, int *l2_associativity)
{
  execute_node *temp;
  fifo_node *fifo_temp = fifo_front;
  char mem_address[9]="";
  int block_size=*blk_size;
  int l1_size=*l1_cache_size;
  int l1_assoc=*l1_associativity;
  int l2_size=*l2_cache_size;
  int l2_assoc=*l2_associativity;
  temp = (execute_node *)malloc(sizeof(execute_node));

  temp->instn_tag = *instn_tag;
  do
  {
    if(fifo_temp->instn_tag == *instn_tag)
    {
      strcpy(mem_address, fifo_temp->mem_address);
      if(fifo_temp->operation_type == 0) temp->cycle = 1;
      else if(fifo_temp->operation_type == 1) temp->cycle = 2;
      else if(fifo_temp->operation_type == 2) temp->cycle = cacheFactory(block_size, l1_size, l1_assoc, l2_size, l2_assoc, mem_address);
      else
      {
        printf("Invalid operation type\n");
        exit(0);
      }
    }
    fifo_temp = fifo_temp->next;
  }while(fifo_temp != fifo_front);

  if(execute_front == NULL)
  {
    execute_front = temp;
    execute_rear = execute_front;
    execute_rear->next = NULL;
  }
  else
  {
    execute_rear->next = temp;
    execute_rear = temp;
    execute_rear->next = NULL;
  }
  execute_count++;

}

void advance_state2EX(int *instn_tag, int *cycle)
{
  //printf("in advnce to EX routine\n");
  execute_node *execute_temp = execute_front;
  fifo_node *fifo_temp = fifo_front;

  while(execute_temp != NULL)
  {
    if(execute_temp->instn_tag == *instn_tag)
    {
      do
      {
        if(fifo_temp->instn_tag == execute_temp->instn_tag)
        {
          strcpy(fifo_temp->instn_state, "EX");
          fifo_temp -> EX_start_cycle = *cycle;
          //printf("FIFO:New state of %s is %s\n", fifo_temp->pc_address, fifo_temp->instn_state);
          //printf("FIFO:EX start cycle: %d\n", fifo_temp->EX_start_cycle);
        }
        fifo_temp = fifo_temp->next;
      }while(fifo_temp != fifo_front);
    }
    execute_temp = execute_temp->next;
  }
}

void execute(int *cycle)
{
  execute_node *execute_temp = execute_front;
  execute_node *deleted_node;
  execute_node *temp = execute_front;
  execute_node *prev;
  issue_node *issue_temp = issue_front;
  issue_node *issue_temp1 = issue_front;
  issue_node *issue_temp2 = issue_front;
  fifo_node *fifo_temp = fifo_front;
  int finished_tag = -1;
  int count=0;
  int ready_tag = 0;

  while(execute_temp != NULL)
  {
    temp = execute_front;
    execute_temp->cycle--;
    if(execute_temp->cycle == 0)
    {
      //printf("tag:%d has completed\n", execute_temp->instn_tag);
      finished_tag = execute_temp->instn_tag;
      do
      {
        if(fifo_temp->instn_tag == execute_temp->instn_tag)
        {
          fifo_temp->completed = true;
          strcpy(fifo_temp->instn_state, "WB");
          fifo_temp -> WB_start_cycle = *cycle;
          //printf("FIFO:New state of %s is %s\n", fifo_temp->pc_address, fifo_temp->instn_state);
          //printf("FIFO:WB start cycle: %d\n", fifo_temp->WB_start_cycle);
          //break;
        }
        fifo_temp = fifo_temp->next;
      }while(fifo_temp != fifo_front);
    }
    else
    {
      //printf("tag %d is still processing\n",execute_temp->instn_tag);
      finished_tag = -1;
    }

    if(finished_tag == -1)
    {
      //printf("No tags have completed(execute delete)\n");
    }
    else
    {
      //delete that tag from execute

      if(execute_temp == execute_front)
      {
        deleted_node = execute_front;
        //printf("deleting tag %d from execute\n", execute_front->instn_tag);
        execute_front = execute_front->next;
        free(deleted_node);
        execute_count--;
      }
      else
      {
        while(temp != NULL && execute_temp->instn_tag != temp->instn_tag)
        {
          prev = temp;
          //printf("tag %d is not deleted\n", prev->instn_tag);
          temp = temp->next;
        }
        //deleted_node = temp;
        prev->next = temp->next;
        if(temp->next == NULL)
        {
          execute_rear = prev;
          //printf("execute_rear is updated\n");
        }
        //printf("execute_rear.tag is %d\n", execute_rear->instn_tag);
        //temp->next = NULL;
        //printf("deleting tag %d from execute\n", temp->instn_tag);
        free(temp);
        execute_count--;
      }
      /*
      printf("tag %d has completed(count is %d)\n", finished_tag, count);
      if(count == 0)
      {
        deleted_node = execute_front;
        printf("deleting tag %d from execute\n", execute_front->instn_tag);
        execute_front = execute_front->next;
        free(deleted_node);
        execute_count--;
      }
      else if(count == 1)
      {
        deleted_node = execute_front->next;
        execute_front->next = deleted_node->next;
        deleted_node->next = NULL;
        printf("deleting tag %d from execute\n", deleted_node->instn_tag);
        free(deleted_node);
        execute_count--;
      }
      else
      {
        temp = execute_front;
        for(int i=0; i<count-1; i++)
        {
          printf("tag not being deleted: %d\n", temp->instn_tag);
          temp = temp->next;

        }
        deleted_node = temp->next;
        temp->next = deleted_node->next;
        if(deleted_node->next == NULL) execute_rear = temp;
        deleted_node->next = NULL;
        printf("deleting tag %d from execute\n", deleted_node->instn_tag);
        free(deleted_node);
        execute_count--;
      }
      */
    }
    if(finished_tag == -1)
    {
      //printf("No tags have completed(update issue list)\n");
    }
    else
    {
      //update issue list with result
      issue_temp = issue_front;
      while(issue_temp != NULL)
      {
        if(issue_temp->src1_tag == finished_tag && issue_temp->src1_rdy_bit == 0)
        {
          issue_temp->src1_rdy_bit = 1;
          //printf("src1 rdy now\n");
        }

        if(issue_temp->src2_tag == finished_tag && issue_temp->src2_rdy_bit == 0)
        {
          issue_temp->src2_rdy_bit = 1;
          //printf("src2 rdy now\n");
        }

        issue_temp = issue_temp->next;
      }
      //update register_file with result

      for(int i=0; i<128; i++)
      {
        if(register_file[i].instn_tag == finished_tag) register_file[i].valid = 1;
      }

    }

    count++;
    //else printf("tag :%d is processing", execute_temp->instn_tag);
    execute_temp = execute_temp->next;
  }
}
void del_from_execute()
{
  return;
}

bool isExecuteFull(int *execute_limit)
{
  if(execute_count == *execute_limit)
  {
    return true;
  }
  else
  {
    return false;
  }
}
bool isExecuteEmpty()
{
  if(execute_count <= 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}
void enqueue(int *operation_type, int *dest_reg_no, int *src1_reg_no, int *src2_reg_no, char *mem_address,
  char *pc_address, int *instn_tag, int *cycle)
{
  if(fifo_count == MAX_FIFO_SIZE)
  {
    //printf("FIFO full\n");
    return;
  }
  else
  {
    fifo_node *temp;

    temp = (fifo_node *)malloc(sizeof(fifo_node));
    if(temp == NULL)
    {
      printf("Insuffient memory\n");
      exit(0);
    }

    temp->operation_type = *operation_type;
    temp->dest_reg_no = *dest_reg_no;
    temp->src1_reg_no = *src1_reg_no;
    temp->src2_reg_no = *src2_reg_no;
    strcpy(temp->mem_address, mem_address);
    strcpy(temp->pc_address, pc_address);
    temp->instn_tag = *instn_tag;
    temp->IF_start_cycle = *cycle;
    temp->ID_start_cycle = 0;
    temp->IS_start_cycle = 0;
    temp->EX_start_cycle = 0;
    temp->WB_start_cycle = 0;
    temp->completed = false;

    strcpy(temp->instn_state, "IF");

    if(fifo_front == NULL)
    {
      fifo_front = temp;
      fifo_rear = fifo_front;
      fifo_rear->next = fifo_front;
    }
    else
    {
      fifo_rear->next = temp;
      fifo_rear = temp;
      temp->next = fifo_front;
    }
    fifo_count++;
  }
}

bool modify_fifo_state(int *tag, char *new_inst_state)
{
  //search for same pc_add and same tag (coz two instn of same add can be present) and overwrite the state with the new state
  //TODO:also should take care of setting start and end time
  bool status = false;
  fifo_node *temp = fifo_front;
  do
  {
    if(temp->instn_tag == *tag)
    {
      status = true;
      //printf("Updating the state of inst of pc_add %s from %s to %s\n", temp->pc_address, temp->instn_state, new_inst_state);
      strcpy(temp->instn_state, new_inst_state);
      if(strcpy(temp->instn_state, new_inst_state)==NULL) printf("Failed to update!\n");
      break;
    }
    temp = temp->next;
  }while(temp != fifo_front);

  return status;
}

void fake_retire()
{
  fifo_node *fifo_temp = fifo_front;
  fifo_node *del_temp = fifo_front;
  fifo_node *deleted_node;
  int IF_cycle_count=0, ID_cycle_count=0, IS_cycle_count=0;
  int EX_cycle_count=0, WB_cycle_count=0;
  int count=0;
  if((fifo_front == fifo_rear) && (fifo_rear == NULL))
  {
    //printf("Queue already empty\n");
    return;
  }
  else
  {
      while(strcmp(fifo_front->instn_state,"WB")==0)
      {
        IF_cycle_count = fifo_front->ID_start_cycle - fifo_front->IF_start_cycle;
        ID_cycle_count = fifo_front->IS_start_cycle - fifo_front->ID_start_cycle;
        IS_cycle_count = fifo_front->EX_start_cycle - fifo_front->IS_start_cycle;
        EX_cycle_count = fifo_front->WB_start_cycle - fifo_front->EX_start_cycle;
        WB_cycle_count = 1;

        printf("%d fu{%d} src{%d,%d} dst{%d} IF{%d,%d} ID{%d,%d} IS{%d,%d} EX{%d,%d} WB{%d,%d}\n",
              fifo_front->instn_tag, fifo_front->operation_type, fifo_front->src1_reg_no, fifo_front->src2_reg_no,
              fifo_front->dest_reg_no, fifo_front->IF_start_cycle, IF_cycle_count, fifo_front->ID_start_cycle,
              ID_cycle_count, fifo_front->IS_start_cycle, IS_cycle_count, fifo_front->EX_start_cycle, EX_cycle_count,
              fifo_front->WB_start_cycle, WB_cycle_count);

        deleted_node = fifo_front;
        //printf("deleting tag %d from fifo\n", fifo_front->instn_tag);

        if(fifo_count == 1)
        {
          deleted_node = fifo_rear;
          free(deleted_node);
          fifo_count--;
          //printf("last.Fifo count is %d\n", fifo_count);
          fifo_rear= NULL;
          fifo_front = NULL;
          return;
        }
        else
        {

          fifo_rear->next = fifo_front->next;
          fifo_front = fifo_front->next;
          //printf("new starting tag of fifo: %d\n", fifo_front->instn_tag);
          deleted_node->next = NULL;
          fifo_count--;
          //printf("Fifo count is %d\n", fifo_count);
          if(fifo_count == 0)
          {
            fifo_front = fifo_rear;
            fifo_rear = NULL;
            return;
          }
          //if(deleted_node == NULL) return 1;
          free(deleted_node);
          fifo_temp=fifo_front;
        }



        /*
        printf("\n****************Fake_ROB Contents(in fake_retire func)****************\n");
        do
        {
          if(fifo_temp != NULL)
          {
            printf("\ninst#: %d\n", fifo_temp->instn_tag);
            printf("op_type: %d ", fifo_temp->operation_type);
            printf("dest_reg#: %d ", fifo_temp->dest_reg_no);
            printf("src1_reg#: %d ", fifo_temp->src1_reg_no);
            printf("src2_reg#: %d ", fifo_temp->src2_reg_no);
            printf("mem_add: %s ", fifo_temp->mem_address);
            printf("pc_add: %s ", fifo_temp->pc_address);
            printf("inst_state: %s\n", fifo_temp->instn_state);
            fifo_temp = fifo_temp->next;
          }
          else exit(0);//printf("Queue is empty\n");
        }while(fifo_temp != fifo_front);
        printf("first tag of fifo: %d\n", fifo_temp->instn_tag);
        */
      }
      /*
      else
      {
        return;
      }
      */
  }


}

fifo_node* dequeue()
{
  fifo_node *temp;
  temp = fifo_front;

  if((fifo_front == fifo_rear) && (fifo_rear == NULL))
  {
    //printf("Queue already empty\n");
    return NULL;
  }
  else
  {
    if(fifo_front == fifo_rear)
    {
      //printf("Queue has been emptied\n");
      fifo_front = NULL;
      fifo_rear = NULL;
    }
    else
    {
      //printf("Dequeuing first entry of queue\n");
      fifo_front = fifo_front->next;
      fifo_rear->next = fifo_front;
      if(fifo_count > 0) fifo_count--;
    }
  }

  return temp;
}

void display()
{
  fifo_node *fifo_temp = fifo_front;
  dispatch_node *dispatch_temp = dispatch_front;
  dispatch_issue_node *dispatch_issue_temp = dispatch_issue_front;
  issue_node *issue_temp = issue_front;
  issue_execute_node *issue_execute_temp = issue_execute_front;
  execute_node *execute_temp = execute_front;
  printf("\n****************Fake_ROB Contents****************\n");
  do
  {
    if(fifo_temp != NULL)
    {
      printf("\ninst#: %d\n", fifo_temp->instn_tag);
      printf("op_type: %d ", fifo_temp->operation_type);
      printf("dest_reg#: %d ", fifo_temp->dest_reg_no);
      printf("src1_reg#: %d ", fifo_temp->src1_reg_no);
      printf("src2_reg#: %d ", fifo_temp->src2_reg_no);
      printf("mem_add: %s ", fifo_temp->mem_address);
      printf("pc_add: %s ", fifo_temp->pc_address);
      printf("inst_state: %s\n", fifo_temp->instn_state);
      fifo_temp = fifo_temp->next;
    }
    else return;
  }while(fifo_temp != fifo_front);
  printf("first tag of fifo: %d\n", fifo_temp->instn_tag);
  printf("\n****************Dispatch List Contents****************\n");
  while(dispatch_temp != NULL)
  {

    if(dispatch_temp != NULL)
    {
      printf("\ntag: %d\n",dispatch_temp->instn_tag);
      printf("inst_state: %s\n", dispatch_temp->instn_state);
      dispatch_temp = dispatch_temp->next;
    }
    else printf("Dispatch is empty\n");
  }
  printf("\n****************Dispatch-Issue Agent List Contents****************\n");
  while(dispatch_issue_temp != NULL)
  {

    if(dispatch_issue_temp != NULL)
    {
      printf("\ntag: %d\n",dispatch_issue_temp->instn_tag);
      dispatch_issue_temp = dispatch_issue_temp->next;
    }
    else printf("Dispatch-Issue Agent is empty\n");
  }
  printf("\n****************Issue List Contents****************\n");
  while(issue_temp != NULL)
  {

    if(issue_temp != NULL)
    {
      printf("\ntag: %d\n", issue_temp->instn_tag);
      printf("src1_tag: %d ", issue_temp->src1_tag);
      printf("src1_rdy_bit: %d ", issue_temp->src1_rdy_bit);
      printf("src2_tag: %d ", issue_temp->src2_tag);
      printf("src2_rdy_bit: %d \n", issue_temp->src2_rdy_bit);
      issue_temp = issue_temp->next;
    }
    else printf("Issue is empty\n");
  }
  printf("\n****************Issue-Execute Agent List Contents****************\n");
  while(issue_execute_temp != NULL)
  {

    if(issue_execute_temp != NULL)
    {
      printf("\ntag: %d\n",issue_execute_temp->instn_tag);
      issue_execute_temp = issue_execute_temp->next;
    }
    else printf("Issue-Execute Agent is empty\n");
  }
  printf("\n****************Execute List Contents****************\n");
  while(execute_temp != NULL)
  {
    if(execute_temp != NULL)
    {
      printf("\ntag: %d\n",execute_temp->instn_tag);
      printf("cycle: %d\n", execute_temp->cycle);
      execute_temp = execute_temp->next;
    }
    else printf("Execute is empty\n");
  }
}


bool advance_cycle()
{
  return false;
}
int main(int argc, char *argv[])
{
  FILE *trace;
  int tag=0;
  int i=0;
  char new_inst_state[3];
  int issue_limit = atoi(argv[1]);
  int n_parameter=atoi(argv[2]);
  int blk_size = atoi(argv[3]);
  int l1_size = atoi(argv[4]);
  int l1_assoc = atoi(argv[5]);
  int l2_size = atoi(argv[6]);
  int l2_assoc = atoi(argv[7]);
  int dispatch_limit=2*n_parameter;
  int execute_limit = n_parameter;
  trace = fopen(argv[8], "r");
  if(trace == NULL) printf("error");
  fseek(trace, 0, SEEK_SET);
  int cycle = 0;
  int operation_type=0;
  int dest_reg_no=0;
  int src1_reg_no=0;
  int src2_reg_no=0;
  int tag2issue=0;
  int tag2execute=0;
  int ID_instn_count = 0;
  char mem_address[9]="";
  char pc_address[9]="";
  int fifo_cleared = 0;
  for(int i=0; i<128; i++)
  {
    register_file[i].instn_tag = 0;
    register_file[i].valid = 1;
  }

  do
  {

  //-----------Fake_Retire begins--------------//
  fake_retire();
  //-----------Fake_Retire ends----------------//


  //-----------Execute begins--------------//
    execute(&cycle);



  //-----------Execute ends----------------//

  //-----------Issue begins--------------//
    //while(number_of_ready_instn_in_issue > 0) do:
    move_from_issue(&n_parameter);
    sort_issue_execute_agent();
    execute_count = 0;
    while(execute_count < n_parameter && issue_execute_count != 0)
    {
      //printf("\nissue execute count is %d\n",issue_execute_count);
      tag2execute = del_from_issue_execute_agent();
      if(tag2execute == -1) exit(0);
      //printf("tag2execute: %d\n", tag2execute);
      del_from_issue(&tag2execute);
      add2execute(&tag2execute, &blk_size, &l1_size, &l1_assoc, &l2_size, &l2_assoc);
      advance_state2EX(&tag2execute, &cycle);
      //printf("finished advcne routine\n");
    }

    clear_issue_execute_agent();//reset the temp list for next time

  //-----------Issue ends----------------//


  //----------Dispatch begins--------------//
    //TODO: all ID state instn of dispatch list are pushed to temp list for sorting.
    //while(number_of_ID_in_dispatch != 0 ) do:
        //modelling the function in while:
        //int number_of_ID_in_dispatch(fifo_node *, dispatch_node *)
        //while(fifo_temp != fifo_front)...actually a "do..while" loop
        //if (*fifo_temp->tag == *dispatch_temp->tag)
        //if(strcmp(*fifo_temp->instn_state, "ID\0")==0) count++;
        //return count;
    move_from_dispatch(&n_parameter);//need to know instn_state of the tag in order to delete

    //while ends
    sort_dispatch_issue_agent();
    //while starts again
    //if issue list is NOT full only then
    //printf("dispatch_issue_count: %d\n",dispatch_issue_count);
    //printf("is agent empty: %d\n",isDispatchIssueAgentEmpty());
    while(issue_count < issue_limit && dispatch_issue_count != 0)
    {
      //printf("inside while loop with issue limit %d\n", issue_limit);
      tag2issue = del_from_dispatch_issue_agent();
      if(tag2issue == -1)
      {
        printf("invalid tag2issue\n");exit(0);
      }
      //printf("tag2issue: %d\n", tag2issue);
      del_from_dispatch(&tag2issue);
      add2issue(&tag2issue, &issue_limit);
      advance_state2IS(&tag2issue, &cycle); //updating rob with IS state using the tag from issue list
      //printf("issue count: %d,dispatch_issue_count: %d\n", issue_count,dispatch_issue_count);
    }

    clear_dispatch_issue_agent();//reset the temp list for next time

    //modify_fifo_state(,,"IS\0");//modify the fifo contents corresponding to tag in issue list

    //modify_fifo_state(,, "ID\0");//modify the fifo contents corresponding to the tag in dispatch list
    //modify_dispatch_state(,, "ID\0");// modify remaining contents of dispatch list to ID.

    // while ends

  //----------Dispatch ends--------------//

  //----------Fetch begins--------------//
  advance_state2ID(&cycle);

    for(i=0;i<n_parameter;i++)
    {
      if(fifo_count != MAX_FIFO_SIZE)
      {
        if(dispatch_count != dispatch_limit)
        {
          if(fgetc(trace) != EOF)
          {
            fseek(trace, -1, SEEK_CUR);
            fscanf(trace, "%s %d %d %d %d %s\n", pc_address, &operation_type, &dest_reg_no, &src1_reg_no, &src2_reg_no, mem_address);
            enqueue(&operation_type, &dest_reg_no, &src1_reg_no, &src2_reg_no, mem_address, pc_address, &tag, &cycle);
            strcpy(new_inst_state, "IF");
            //modify_fifo_state(&tag, new_inst_state);
            add2dispatch(&tag, new_inst_state, &dispatch_limit);
            tag++;
          }
        }
      }
    }


//----------Fetch ends--------------//

//display();
//printf("Fifo_count: %d, execute_count: %d, issue_count: %d, dispatch_count : %d\n",fifo_count,execute_count,issue_count, dispatch_count);
cycle++;
}while(fifo_count != 0 || execute_count != 0 || issue_count != 0 || dispatch_count != 0);

printf("CONFIGURATION\n");
printf("superscalar bandwidth (N) = %d\n",n_parameter);
printf("dispatch queue size (2*N) = %d\n",dispatch_limit);
printf("schedule queue size (S)   = %d\n",issue_limit);
printf("RESULTS\n");
printf("number of instructions = %d\n",tag);
printf("number of cycles       = %d\n",cycle-1);
printf("IPC                    = %0.2f\n",float(tag)/float(cycle-1));

}
