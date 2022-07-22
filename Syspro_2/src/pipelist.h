#ifndef PIPELIST_H
#define PIPELIST_H

// records get inserted in this COMMON list

struct Record_list {
  char *pipeName; // key
  int fd;
  struct Record_list *next;

};

struct Record_list *Create_Record(struct Record_list *list_ptr, char *pipename);

void Destroy_Record_list(struct Record_list *);

void Print_Record_List(struct Record_list *);

void Create_Pipes_From_List(struct Record_list *);

void Destroy_Pipes_From_List(struct Record_list *);
#endif