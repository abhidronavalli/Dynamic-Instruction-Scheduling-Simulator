/*Cache simulator for ECE563*/
//Part A: Includes only L1 Cache hierarchy with LRU, LFU read policies and WBWA and WTWNA policies
//Part B: Includes L2 Cache along with victim Cache


#include "cacheFactory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include <stdbool.h>
#include <limits.h>
//#define BLOCK_SIZE 256
#define FALSE 0
#define TRUE 1


int i=0,j=0,temp1=0,temp2=0,temp3=0,temp4=0,temp9=0,temp10=0,temp11=0,temp12=0,l2_temp1=0;
int base=1, status = 0;
int hit_flag=0;
int l2_hit_flag=0;
int read_hit_count=0, read_miss_count=0, traffic=0;
int l2_read_hit_count=0, l2_read_miss_count=0, l2_traffic=0;

int cache_blocks_number=0;
int l2_cache_blocks_number=0;
int block_offset=0;
int block_index=0;
int l2_block_index=0;
int block_tag=0;
int l2_block_tag=0;
int sets=0;
int l2_sets=0;

static char (*set_mapped_cache_tag_arrays)[33]=NULL;
static int *set_mapped_cache_valid_arrays=NULL;
int *lru_counter=NULL;

char (*l2_set_mapped_cache_tag_arrays)[33]=NULL;
int *l2_set_mapped_cache_valid_arrays=NULL;
int *l2_lru_counter=NULL;

char read_write = 'r';
char cpu_address_hex[9]="";
char policy[4] = "LRU";

char cpu_address_binary[33]="";
char cpu_address_binary_temp_nibble[5]="";
char one_zero[9] = "0";
char two_zero[9] = "00";
char three_zero[9] = "000";
char four_zero[9] = "0000";
char five_zero[9] = "00000";
char six_zero[9] = "000000";
char seven_zero[9] = "0000000";
char tag_bits[33]="";
char l2_tag_bits[33]="";
char index_bits[33]="";
char l2_index_bits[33]="";
char index_bits_copied[33]="";
char l2_index_bits_copied[33]="";
int index_bits_decimal=0;
int l2_index_bits_decimal=0;
int index_bits_div_10=0;
int l2_index_bits_div_10=0;
char block_offset_bits[33]="";
char l2_block_offset_bits[33]="";

int cacheFactory(int block_size, int l1_size, int l1_assoc, int l2_size, int l2_assoc, char *mem_address)
{
  //variable declaration
  int cache_block_size = block_size;
  int l1_cache_size = l1_size;
  int l2_cache_size = l2_size;
  strcpy(cpu_address_hex, mem_address);

  printf("===== Simulator configuration =====\n");
  printf("L1_BLOCKSIZE:                    %d\n",cache_block_size);
  printf("L1_SIZE:                         %d\n",l1_cache_size);
  printf("L1_ASSOC:                        %d\n",l1_assoc);
  printf("L2_SIZE:                         %d\n",l2_cache_size);
  printf("L2_ASSOC:                        %d\n",l2_assoc);
  printf("Mem address:                     %s\n",cpu_address_hex);
  printf("Replacement Policy:              %s\n",policy);
  printf("===================================\n");
  printf("\n");

  //logic
  cache_blocks_number = l1_cache_size/(l1_assoc*cache_block_size);
  block_offset = floor(log2(cache_block_size));
  block_index = floor(log2(cache_blocks_number));
  block_tag = 32 - (block_offset + block_index);
  sets = l1_assoc;
  printf("No. of Tag bits = %d\n",block_tag);
  printf("No. of Index bits = %d\n",block_index);
  printf("No. of Block Offset bits = %d\n",block_offset);

  set_mapped_cache_tag_arrays = (char(*)[33])malloc(sizeof(char[33])*cache_blocks_number*sets);
  if(set_mapped_cache_tag_arrays == NULL) printf("Cache init failed!\n");
  //else printf("Cache init success!\n");
  //initialising the tag array with zeros
  for(i=0;i<sets;i++)
  {
      for(j=0;j<cache_blocks_number;j++)
      {
          memset(set_mapped_cache_tag_arrays[(i*cache_blocks_number) + j], 0, 33*sizeof(char));

      }
  }

  set_mapped_cache_valid_arrays = (int *)malloc(sizeof(int)*cache_blocks_number*sets);
  if(set_mapped_cache_valid_arrays == NULL) printf("Cache valid array init failed!\n");
  //else printf("Cache init success!\n");
  //initialising the tag array with zeros
  for(i=0;i<sets;i++)
  {
      for(j=0;j<cache_blocks_number;j++)
      {
          set_mapped_cache_valid_arrays[(i*cache_blocks_number) + j] = 0;

      }
  }

  lru_counter = (int*)malloc(sizeof(int)*cache_blocks_number*sets);
  if(lru_counter == NULL) printf("LRU counter init failed!\n");
  //else printf("LRU counter init success!\n");
  //adding random values to lru_counter for initial setup
  temp1 = sets-1;
  for(i=0;i<sets;i++)
  {
      for(j=0;j<cache_blocks_number;j++)
      {
          lru_counter[(i*cache_blocks_number) + j]=temp1;

      }
      temp1 = temp1 - 1;
  }
  temp1=0;



  if(l2_cache_size !=0 && l2_assoc != 0)
  {
    l2_cache_blocks_number = l2_cache_size/(l2_assoc*cache_block_size);
    l2_block_index = floor(log2(l2_cache_blocks_number));
    l2_block_tag = 32 - (block_offset + l2_block_index);
    l2_sets = l2_assoc;
    printf("no of L2 tag bits: %d\n",l2_block_tag);
    printf("no of L2 index bits: %d\n",l2_block_index);

    l2_set_mapped_cache_tag_arrays = (char(*)[33])malloc(sizeof(char[33])*l2_cache_blocks_number*l2_sets);
    if(l2_set_mapped_cache_tag_arrays == NULL) printf("L2 Cache init failed!\n");
    //else printf("L2 Cache init success!\n");
    //initialising the tag array with zeros
    for(i=0;i<l2_sets;i++)
    {
        for(j=0;j<l2_cache_blocks_number;j++)
        {
            memset(l2_set_mapped_cache_tag_arrays[(i*l2_cache_blocks_number) + j], 0, 33*sizeof(char));

        }
    }

    l2_set_mapped_cache_valid_arrays = (int *)malloc(sizeof(int)*l2_cache_blocks_number*l2_sets);
    if(l2_set_mapped_cache_valid_arrays == NULL) printf("L2 Cache valid array init failed!\n");
    //else printf("L2 Cache init success!\n");
    //initialising the tag array with zeros
    for(i=0;i<l2_sets;i++)
    {
        for(j=0;j<l2_cache_blocks_number;j++)
        {
            l2_set_mapped_cache_valid_arrays[(i*l2_cache_blocks_number) + j] = 0;

        }
    }

    l2_lru_counter = (int*)malloc(sizeof(int)*l2_cache_blocks_number*l2_sets);
    if(l2_lru_counter == NULL) printf("L2 LRU counter init failed!\n");
    //else printf("LRU counter init success!\n");
    //adding random values to lru_counter for initial setup
    l2_temp1 = l2_sets - 1;
    for(i=0;i<l2_sets;i++)
    {
        for(j=0;j<l2_cache_blocks_number;j++)
        {
            l2_lru_counter[(i*l2_cache_blocks_number) + j]=l2_temp1;

        }
        l2_temp1 = l2_temp1 - 1;
    }
    l2_temp1=0;
  }


  if(strlen(cpu_address_hex)==7)
  {
      strcat(one_zero,cpu_address_hex);
      strcpy(cpu_address_hex,one_zero);
      strcpy(one_zero,"0");
  }
  else if(strlen(cpu_address_hex)==6)
  {
      strcat(two_zero,cpu_address_hex);
      strcpy(cpu_address_hex,two_zero);
      strcpy(two_zero,"00");
  }
  else if(strlen(cpu_address_hex)==5)
  {
      strcat(three_zero,cpu_address_hex);
      strcpy(cpu_address_hex,three_zero);
      strcpy(three_zero,"000");
  }
  else if(strlen(cpu_address_hex)==4)
  {
      strcat(four_zero,cpu_address_hex);
      strcpy(cpu_address_hex,four_zero);
      strcpy(four_zero,"0000");
  }
  else if(strlen(cpu_address_hex)==3)
  {
      strcat(five_zero,cpu_address_hex);
      strcpy(cpu_address_hex,five_zero);
      strcpy(five_zero,"00000");
  }
  else if(strlen(cpu_address_hex)==2)
  {
      strcat(six_zero,cpu_address_hex);
      strcpy(cpu_address_hex,six_zero);
      strcpy(six_zero,"000000");
  }
  else if(strlen(cpu_address_hex)==1)
  {
      strcat(seven_zero,cpu_address_hex);
      strcpy(cpu_address_hex,seven_zero);
      strcpy(seven_zero,"0000000");
  }
  else if(strlen(cpu_address_hex)==0)
  {
      //printf("No address present at location %ld of the input file",line);
      printf("Exiting...........");
      exit(EXIT_FAILURE);
  }
  printf("CPU Address in Hex (with zero tagging)= %s\n",cpu_address_hex);
  for(i=0;i<=strlen(cpu_address_hex)-1;i++)
  {
      if(cpu_address_hex[i]=='0') strcpy(cpu_address_binary_temp_nibble,"0000");
      else if(cpu_address_hex[i]=='1') strcpy(cpu_address_binary_temp_nibble,"0001");
      else if(cpu_address_hex[i]=='2') strcpy(cpu_address_binary_temp_nibble,"0010");
      else if(cpu_address_hex[i]=='3') strcpy(cpu_address_binary_temp_nibble,"0011");
      else if(cpu_address_hex[i]=='4') strcpy(cpu_address_binary_temp_nibble,"0100");
      else if(cpu_address_hex[i]=='5') strcpy(cpu_address_binary_temp_nibble,"0101");
      else if(cpu_address_hex[i]=='6') strcpy(cpu_address_binary_temp_nibble,"0110");
      else if(cpu_address_hex[i]=='7') strcpy(cpu_address_binary_temp_nibble,"0111");
      else if(cpu_address_hex[i]=='8') strcpy(cpu_address_binary_temp_nibble,"1000");
      else if(cpu_address_hex[i]=='9') strcpy(cpu_address_binary_temp_nibble,"1001");
      else if(cpu_address_hex[i]=='a') strcpy(cpu_address_binary_temp_nibble,"1010");
      else if(cpu_address_hex[i]=='b') strcpy(cpu_address_binary_temp_nibble,"1011");
      else if(cpu_address_hex[i]=='c') strcpy(cpu_address_binary_temp_nibble,"1100");
      else if(cpu_address_hex[i]=='d') strcpy(cpu_address_binary_temp_nibble,"1101");
      else if(cpu_address_hex[i]=='e') strcpy(cpu_address_binary_temp_nibble,"1110");
      else if(cpu_address_hex[i]=='f') strcpy(cpu_address_binary_temp_nibble,"1111");
      else
          {
              //printf("Invalid memory address at line %ld of input file\n",line);
              printf("Exiting.........\n");
              exit(EXIT_FAILURE);
          }
      //puts(cpu_address_binary_temp_nibble);
      strcat(cpu_address_binary,cpu_address_binary_temp_nibble);
  }

  strncpy(tag_bits,cpu_address_binary,block_tag);
  strncpy(index_bits,cpu_address_binary+block_tag,block_index);
  strncpy(block_offset_bits,cpu_address_binary+block_tag+block_index,block_offset);

  strcpy(index_bits_copied,index_bits);//since index bits is used up below, a copy of it is saved for future uses
  //converting index to decimal
  while(atoi(index_bits) != 0)
  {
      index_bits_decimal = index_bits_decimal + base*(atoi(index_bits)%10);
      index_bits_div_10 = atoi(index_bits) / 10;
      base=base*2;
      snprintf(index_bits, block_index, "%d", index_bits_div_10);
  }
  base=1;
  printf("L1 Index in decimal : %d\n",index_bits_decimal);

  if(l2_cache_size != 0)
  {
    strncpy(l2_tag_bits,cpu_address_binary,l2_block_tag);
    strncpy(l2_index_bits,cpu_address_binary+l2_block_tag,l2_block_index);
    strncpy(l2_block_offset_bits,cpu_address_binary+l2_block_tag+l2_block_index,block_offset);

    strcpy(l2_index_bits_copied,l2_index_bits);//a copy of it is saved for future uses

    while(atoi(l2_index_bits) != 0)
    {
        l2_index_bits_decimal = l2_index_bits_decimal + base*(atoi(l2_index_bits)%10);
        l2_index_bits_div_10 = atoi(l2_index_bits) / 10;
        base=base*2;
        snprintf(l2_index_bits, l2_block_index, "%d", l2_index_bits_div_10);
    }
    base=1;
    printf("L2 Index in decimal : %d\n",l2_index_bits_decimal);
  }

  if(l1_size !=0 && l1_assoc != 0)
  {
    for(j=0;j<sets;j++)
    {
        if(strcmp(set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal],tag_bits)==0)
        {
            hit_flag = 1;
            temp1 = j;
            temp2 = lru_counter[(j*cache_blocks_number) + index_bits_decimal];
            break;
        }
        else hit_flag =0;
    }

    if(hit_flag == 1)
    {
      printf("L1 Cache Hit\n");
      read_hit_count++;
      lru_counter[(temp1*cache_blocks_number) + index_bits_decimal] = 0;
      for(j=0;j<sets;j++)
      {
          if(j!=temp1)
          {
              if((lru_counter[(j*cache_blocks_number) + index_bits_decimal]) < temp2)
              {
                  lru_counter[(j*cache_blocks_number) + index_bits_decimal]++;
                  //printf("incremented LRU counter of set %d\n",j);
              }
          }

      }
      status = 1;
      //break;
    }

    else if(hit_flag == 0)
    {
      //update counter
      //copy tag
      //lru bits=log2(no.of sets)
      //check for lru value
      //strcpy to the highest value lru flag
      //set that lru to zero
      //inc all other lru counter values by 1
      //request data from lower level caches
      printf("L1 Cache Miss\n");
      temp3 = 0;
      temp4 = 0;
      //searching for the highest LRU counter value and setting it to zero
      for(j=0;j<sets;j++)
      {
          if(lru_counter[(j*cache_blocks_number) + index_bits_decimal] > temp3)
          {
              temp3 = lru_counter[(j*cache_blocks_number) + index_bits_decimal];
              temp4 = j;
          }
      }

      strcpy(set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal],tag_bits);
      if(strcpy(set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal],tag_bits) ==NULL) exit(EXIT_FAILURE);

      lru_counter[(temp4*cache_blocks_number) + index_bits_decimal] = 0;
      for(j=0;j<sets;j++)
      {
          if(j!=temp4)
          lru_counter[(j*cache_blocks_number) + index_bits_decimal]++;

      }

      for(j=0;j<sets;j++)
      {
          //printf("Updated Tag address at set %d: %s\n",j,set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal]);
      }
      //except for the recently modified lru counter, rest all counters are incremented
      //printf("LRU Counter value at set %d of index %d: %d\n",temp4,index_bits_decimal,lru_counter[(temp4*cache_blocks_number) + index_bits_decimal]);
      if(read_write == 'r')
      {
          read_miss_count++;
          traffic++;
      }
      //printf("Updated values of LRU counters\n");
      for(j=0;j<sets;j++)
      {
          //printf("Set %d: %d\n",j,lru_counter[(j*cache_blocks_number) + index_bits_decimal]);
      }

      //now check if L2 is enabled and start L2 routine
      if(l2_cache_size != 0)
      {
        printf("L2 Cache enabled\n");
        //now send a read request to L2 Cache
        for(j=0;j<l2_sets;j++)
        {
          if(strcmp(l2_set_mapped_cache_tag_arrays[(j*l2_cache_blocks_number) + l2_index_bits_decimal],l2_tag_bits)==0)
          {
            l2_hit_flag = 1;
            temp9 = j;
            temp10 = l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal];
            break;
          }
          else l2_hit_flag = 0;
        }

        if(l2_hit_flag == 1)
        {
          printf("L2 Cache Hit\n");
          l2_read_hit_count++;
          l2_lru_counter[(temp9*l2_cache_blocks_number) + l2_index_bits_decimal] = 0;
          for(j=0;j<l2_sets;j++)
          {
              if(j!=temp9)
              {
                  if((l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal]) < temp10)
                  {
                      l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal]++;
                      //printf("incremented LRU counter of set %d\n",j);
                  }
              }

          }
          status=2;
          //break;
        }

        else if(l2_hit_flag == 0)
        {
          printf("L2 Cache Miss\n");
          temp11 = 0;
          temp12 = 0;
          //searching for the highest LRU counter value and setting it to zero
          for(j=0;j<l2_sets;j++)
          {
              if(l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal] > temp11)
              {
                  temp11 = l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal];
                  temp12 = j;
              }
          }

          l2_read_miss_count++;
          l2_traffic++;
          strcpy(l2_set_mapped_cache_tag_arrays[(temp12*l2_cache_blocks_number) + l2_index_bits_decimal],l2_tag_bits);
          if(strcpy(l2_set_mapped_cache_tag_arrays[(temp12*l2_cache_blocks_number) + l2_index_bits_decimal],l2_tag_bits) ==NULL) exit(EXIT_FAILURE);

          l2_lru_counter[(temp12*l2_cache_blocks_number) + l2_index_bits_decimal] = 0;
          for(j=0;j<l2_sets;j++)
          {
              if(j!=temp12)
              l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal]++;

          }

          for(j=0;j<l2_sets;j++)
          {
              //printf("Updated Tag address at set %d of L2: %s\n",j,l2_set_mapped_cache_tag_arrays[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal]);
          }
          //except for the recently modified lru counter, rest all counters are incremented
          //printf("LRU Counter value at set %d of index %d of L2: %d\n",temp12,evicted_block_index_bits_decimal,l2_lru_counter[(temp12*l2_cache_blocks_number) + evicted_block_index_bits_decimal]);
          //printf("Updated values of LRU counters\n");
          for(j=0;j<l2_sets;j++)
          {
              //printf("Set %d of L2: %d\n",j,l2_lru_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal]);
          }
          status = 3;
          //break;
        }
      }
      else
      {
        printf("L1 Cache Miss\n");
        status = 4;
        //return 20;
      }
    }
  }
  else
  {
    printf("No cache intergration detected\n");
    status = 1;
  }


  memset(cpu_address_binary_temp_nibble,0,sizeof(cpu_address_binary_temp_nibble));
  memset(cpu_address_binary,0,sizeof(cpu_address_binary));
  memset(tag_bits,0,33*sizeof(char));
  memset(index_bits,0,33*sizeof(char));
  memset(block_offset_bits,0,33*sizeof(char));
  index_bits_decimal=0;
  if(l2_sets != 0)
  {
    memset(l2_block_offset_bits,0,33*sizeof(char));
    memset(l2_tag_bits,0,33*sizeof(char));
    memset(l2_index_bits,0,33*sizeof(char));
    l2_index_bits_decimal=0;
  }
  
  if(status == 1)
  {
    printf("returning 5\n");return 5;
  }
  else if(status == 2)
  {
    printf("returning 10\n");return 10;
  }
  else if(status == 3)
  {
    printf("returning 20\n");return 20;
  }
  else if(status == 4)
  {
    printf("returning 20\n");return 20;
  }
  else
  {
    printf("Error in cache factory\n");
    return -1;
  }
}
/*
    //parse routine ends


    //converting di
    //de allocating the index and tag array from Direct mapped cache
    //free(direct_mapped_cache_index_array);

    //if(rep_policy == '0')
    if(rep_policy == '2')
    {

      //temp21 = sets - 1;
      printf("===== L1 contents =====\n");
      for(j=0;j<cache_blocks_number;j++)
      {
          printf("Set %d: ",j);
          //temp21 = sets - 1;
          for(i=0;i<sets;i++)
          {
            //for(k=0;k<sets;k++)
            //{
              //if(lru_counter[(k*cache_blocks_number) + j] == temp21)
              //{
                hex_equ = strtol(set_mapped_cache_tag_arrays[(i*cache_blocks_number) + j],NULL,2);
                printf("%lx ",hex_equ);
                if (set_mapped_dirty_bit[(i*cache_blocks_number) + j]==1) printf("D "); else printf(" ");
              //}
            //}
            //temp21 = temp21 - 1;
          }
          printf("\n");

      }
      if(is_victim_cache_enabled != 0)
      {
          printf("===== Victim Cache contents =====\n");
          for(j=0;j<victim_cache_blocks_number;j++)
          {
              printf("Set %d: ",j);
              for(i=0;i<victim_cache_sets;i++)
              {
                  hex_equ = strtol(set_mapped_victim_cache_tag_arrays[(i*victim_cache_blocks_number) + j],NULL,2);
                  printf("%lx ",hex_equ);
                  if (set_mapped_victim_dirty_bit[(i*victim_cache_blocks_number) + j]==1) printf("D "); else printf(" ");
              }
              printf("\n");

          }
      }
      if(l2_cache_size != 0)
      {
        //temp21 = l2_sets - 1;
        printf("===== L2 contents =====\n");
        for(j=0;j<l2_cache_blocks_number;j++)
        {
            printf("Set %d: ",j);
            //temp21 = l2_sets - 1;
            for(i=0;i<l2_sets;i++)
            {
              //for(k=0;k<l2_sets;k++)
              //{
                //if(l2_lru_counter[(k*l2_cache_blocks_number) + j] == temp21)
                //{
                  hex_equ = strtol(l2_set_mapped_cache_tag_arrays[(i*l2_cache_blocks_number) + j],NULL,2);
                  printf("%lx ",hex_equ);
                  if (l2_set_mapped_dirty_bit[(i*l2_cache_blocks_number) + j]==1) printf("D "); else printf(" ");
                //}
              //}
              //temp21 = temp21 - 1;
            }
            printf("\n");

        }
      }
    }

    //else if(rep_policy == '1')
    else if(rep_policy == '3')
    {

      temp21 = sets - 1;
      printf("===== L1 contents =====\n");
      for(j=0;j<cache_blocks_number;j++)
      {
          printf("Set %d: ",j);
          temp21 = sets - 1;
          for(i=0;i<sets;i++)
          {
            for(k=0;k<sets;k++)
            {
              if(lru_counter[(k*cache_blocks_number) + j] == temp21)
              {
                hex_equ = strtol(set_mapped_cache_tag_arrays[(k*cache_blocks_number) + j],NULL,2);
                printf("%lx ",hex_equ);
                if (set_mapped_dirty_bit[(k*cache_blocks_number) + j]==1) printf("D "); else printf(" ");
              }
            }
            temp21 = temp21 - 1;
          }
          printf("\n");

      }

      if(is_victim_cache_enabled != 0)
      {
          printf("===== Victim Cache contents =====\n");
          for(j=0;j<victim_cache_blocks_number;j++)
          {
              printf("Set %d: ",j);
              for(i=0;i<victim_cache_sets;i++)
              {
                  hex_equ = strtol(set_mapped_victim_cache_tag_arrays[(i*victim_cache_blocks_number) + j],NULL,2);
                  printf("%lx ",hex_equ);
                  if (set_mapped_victim_dirty_bit[(i*victim_cache_blocks_number) + j]==1) printf("D "); else printf(" ");
              }
              printf("\n");

          }
      }

      if(l2_cache_size != 0)
      {
        temp21 = l2_sets - 1;
        printf("===== L2 contents =====\n");
        for(j=0;j<l2_cache_blocks_number;j++)
        {
            printf("Set %d: ",j);
            temp21 = l2_sets - 1;
            for(i=0;i<l2_sets;i++)
            {
              for(k=0;k<l2_sets;k++)
              {
                if(l2_lru_counter[(k*l2_cache_blocks_number) + j] == temp21)
                {
                  hex_equ = strtol(l2_set_mapped_cache_tag_arrays[(k*l2_cache_blocks_number) + j],NULL,2);
                  printf("%lx ",hex_equ);
                  if (l2_set_mapped_dirty_bit[(k*l2_cache_blocks_number) + j]==1) printf("D "); else printf(" ");
                }
              }
              temp21 = temp21 - 1;
            }
            printf("\n");

        }
      }
    }

    free(set_mapped_cache_tag_arrays);
    free(set_mapped_cache_valid_arrays);
    free(lru_counter);
    free(lfu_counter);
    free(lfu_counter_set_age);
    free(set_mapped_dirty_bit);
    if(is_victim_cache_enabled != 0)
    {
        free(set_mapped_victim_cache_tag_arrays);
        free(set_mapped_victim_cache_valid_arrays);
        free(set_mapped_victim_dirty_bit);
        free(victim_cache_lru_counter);
    }
    if(l2_cache_size != 0)
    {
      free(l2_set_mapped_cache_tag_arrays);
      free(l2_lru_counter);
      free(l2_lfu_counter);
      free(l2_lfu_counter_set_age);
      free(l2_set_mapped_dirty_bit);
    }


    miss_rate = (((float)read_miss_count+write_miss_count)/(float)(read_miss_count+read_hit_count+write_hit_count+write_miss_count));
    l2_miss_rate = (((float)l2_read_miss_count+l2_write_miss_count)/(float)(l2_read_miss_count+l2_read_hit_count+l2_write_hit_count+l2_write_miss_count));

    l1_hit_time = 0.25 + (2.5*(total_cache_size/(512.0000*1024.0000))) + (0.025*(cache_block_size/16)) + (0.025*sets);
    l2_hit_time = 2.5 + (2.5*(l2_cache_size/(512.0000*1024.0000))) + (0.025*(cache_block_size/16)) + (0.025*l2_sets);
    //printf("hit time %f\n",l1_hit_time);

    l1_miss_penalty = 20 + (0.5*(cache_block_size/16));
    l2_miss_penalty = 20 + (0.5*(cache_block_size/16));

    //printf("miss penalty %f\n",l1_miss_penalty);

    //l1_total_access_time = ((read_miss_count+read_hit_count+write_hit_count+write_miss_count)*l1_hit_time) +((read_miss_count+write_miss_count)*l1_miss_penalty);
    if(l2_cache_size != 0)
    {
      l2_avg_access_time = l1_hit_time + (miss_rate*(l2_hit_time + (l2_miss_rate*l2_miss_penalty)));
      l2_miss_rate_mod = l2_miss_rate;
      final_traffic = l2_traffic;
    }
    else
    {
      l2_avg_access_time = l1_hit_time + (miss_rate*l1_miss_penalty);
      l2_miss_rate_mod = 0;
      final_traffic = traffic;
    }


    printf("\n====== Simulation results (raw) ======\n");
    printf("a. number of L1 reads:              %ld\n",read_miss_count+read_hit_count);
    printf("b. number of L1 read misses:        %ld\n",read_miss_count);
    printf("c. Number of L1 writes:             %ld\n",write_hit_count+write_miss_count);
    printf("d. number of L1 write misses:       %ld\n",write_miss_count);
    printf("e. L1 miss rate:                    %0.4f\n",miss_rate);
    printf("f. number of swaps:                 %d\n",l1_victim_swaps);
    printf("g. number of victim cache writeback:%ld\n",victim_cache_write_back);
    printf("h. number of L2 reads:              %d\n",l2_read_miss_count+l2_read_hit_count);
    printf("i. number of L2 read misses:        %d\n",l2_read_miss_count);
    printf("j. Number of L2 writes:             %d\n",l2_write_hit_count+l2_write_miss_count);
    printf("k. number of L2 write misses:       %d\n",l2_write_miss_count);
    if(l2_cache_size != 0)
    printf("l. L2 miss rate:                    %0.4f\n",l2_miss_rate_mod);
    else
    printf("l. L2 miss rate:                    0\n");
    printf("m. number of L2 writeback:          %ld\n",l2_write_back);
    printf("n. total memory traffic:            %ld\n",final_traffic);
	printf("\n");
    //L1 miss penalty (in ns) = 20 ns + 0.5*(L1_BLOCKSIZE / 16 B/ns))
    //L1 Cache Hit Time (in ns) = 0.25ns + 2.5ns * (L1_Cache Size / 512kB) + 0.025ns * (L1_BLOCKSIZE / 16B) + 0.025ns * L1_SET_ASSOCIATIVITY
    printf("==== Simulation results (performance) ====\n");
    printf("1. average access time:             %0.4f ns\n",l2_avg_access_time);
    //fflush(trace_file);
    //printf("Closing the trace file\n");

    fclose(trace_file);
    return 0;
*/
