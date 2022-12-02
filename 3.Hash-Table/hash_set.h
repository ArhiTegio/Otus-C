#ifndef HASH_SET_H_INCLUDED
#define HASH_SET_H_INCLUDED

//Структуры

typedef struct listnode listnode;
struct listnode {
    char *key;
    int count_;
    int value;
    struct listnode *prev;
    struct listnode *next;
};

typedef struct hashset hashset;
struct hashset {
    struct listnode *hashtab[1024];
    struct listnode *head;
    int length;
    void (*add)(hashset *self, char *element);
    void (*remove_)(hashset *self, char *element);
    listnode (*contains)(hashset *self, char *element);
};

//Функции
void test();

int foo();
hashset GetHashTable();
unsigned int hash_func(char *s, unsigned int hash_size);

#endif // HASH_SET_H_INCLUDED

