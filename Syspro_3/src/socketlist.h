#ifndef SOCKETLIST_H
#define SOCKETLIST_H

// records get inserted in this COMMON list

struct Record_list {
  int fd;
  struct Record_list *next;

};

struct Record_list *Create_Record(struct Record_list *list_ptr);

void Destroy_Record_list(struct Record_list *);

void Print_Record_List(struct Record_list *);

void Destroy_Sockets_From_List(struct Record_list *);
#endif