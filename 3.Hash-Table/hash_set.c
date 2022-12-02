#include "hash_set.h"

#include <stdio.h>
#include <stdlib.h>

void test()
{
    printf("%s", "Hello test.\n");
}

unsigned int hash_func(char *s, unsigned int hash_size)
{
    size_t len = sizeof(s)/sizeof(*s);
    unsigned int hash = 0, i;
    while(*s)
    {
        hash = (hash << 3) + *s++;
        i = hash & 0xf0000000L;
        if(i)
        {
            hash ^= i >> 24;
        }
        hash &= ~i;
    }

    return hash % hash_size;
}


void addElement(hashset *self, char *element)
{
    int length = sizeof(self->hashtab) / sizeof(*self->hashtab);

    int idx = hash_func(element, length);
    if (idx != NULL)
    {
        struct listnode *node = NULL;
        node = malloc(sizeof(struct listnode));

        node->key = element;
        node->value = idx;
        node->count_ = 1;
        node->next = NULL;
        node->prev = NULL;
        if(self->head != NULL)
        {
            if(self->head->value > node->value)
            {
                node->next = self->head;
                self->head->prev = node;
                node->next = self->head;
                self->head = node;
                self->hashtab[idx] = node;
                self->length++;
            }
            else
            {
                listnode *node_prev;
                for(listnode *pos = self->head; pos != NULL; pos = pos->next)
                {
                    if(pos->value <= idx)
                       node_prev = pos;
                    else
                        break;
                }

                if(node_prev->value == node->value)
                {
                    self->hashtab[node->value]->key = node->key;
                    self->hashtab[node->value]->count_++;
                }
                else
                {
                    node->prev = node_prev;
                    node->next = node_prev->next;
                    node_prev->next = node;
                    self->hashtab[idx] = node;
                    self->length++;
                }
            }
        }
        else
        {
            self->head = node;
            self->hashtab[idx] = node;
            self->length++;
        }
    }
}

void removeElement(hashset *self, char *element)
{
    int length = sizeof(self->hashtab) / sizeof(*self->hashtab);
    int idx = hash_func(element, length);
    if(self->hashtab[idx] != NULL)
    {
        listnode *node;
        if(self->length == 1)
        {
            node = self->hashtab[idx];
            self->hashtab[idx] = NULL;
            if(node->value == self->head->value)
                self->head = NULL;
            free(node);
        }
        else if (self->length > 1)
        {
            node = self->hashtab[idx];
            if(node->value == self->head->value)
            {
                self->head->next->prev = NULL;
                self->head = self->head->next;
                free(node);
            }
            else
            {
                listnode *node_tmp = node->prev;
                node->next->prev = node_tmp;
                node->prev->next = node->next;
                free(node);
            }
        }
        self->length--;
    }

}

listnode* foundElement(hashset *self, char *element)
{
    int idx;
    return self->hashtab[idx];
}

hashset GetHashTable()
{
    struct hashset ahashset;
     ahashset.head = NULL;
    int length = sizeof(ahashset.hashtab) / sizeof(*ahashset.hashtab);
    for (int i = 0; i < length; i++) {
        ahashset.hashtab[i] = NULL;
    }

    ahashset.add = &addElement;
    ahashset.remove_ = &removeElement;
    ahashset.contains = &foundElement;
    return ahashset;
}
