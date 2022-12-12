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

unsigned int hash_func_collision(char *s, unsigned int hash_size)
{
    size_t len = sizeof(s)/sizeof(*s);
    unsigned int hash = 0, i;
    while(*s)
    {
        hash = (hash << 4) + *s++;
        i = hash & 0xf0000000L;
        if(i)
        {
            hash ^= i >> 16;
        }
        hash &= ~i;
    }

    return hash % hash_size;
}


void addElement(hashset *self, char *element)
{
    int length = sizeof(self->hashtab) / sizeof(*self->hashtab);

    int idx = hash_func(element, length);
    int idx_collision = hash_func_collision(element, length);
    if (idx != NULL)
    {
        struct listnode *node = NULL;
        node = malloc(sizeof(struct listnode));

        node->key = element;
        node->value = idx;
        node->count_ = 1;
        node->idx_collision = idx_collision;
        node->next = NULL;
        node->collision = NULL;
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
                    if(node->idx_collision != self->hashtab[node->value]->idx_collision)
                    {
                        if(self->hashtab[node->value]->collision == NULL)
                        {
                            node->prev = self->hashtab[node->value];
                            self->hashtab[node->value]->collision = node;
                            self->length++;
                        }
                        else
                        {
                            struct listnode *node_collision = NULL;
                            node_collision = self->hashtab[node->value]->collision;

                            do
                            {
                                if(node->idx_collision == node_collision->idx_collision)
                                {
                                    node_collision->count_++;
                                    break;
                                }
                                else
                                {
                                    if(node_collision->collision == NULL)
                                    {
                                        node_collision->collision = node;
                                        self->length++;
                                        break;
                                    }
                                    else
                                    {
                                        node_collision = node_collision->collision;
                                    }
                                }
                            }
                            while(1);
                        }
                    }
                    else
                    {
                        self->hashtab[node->value]->count_++;
                    }
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
    int idx_collision = hash_func_collision(element, length);
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
                if(node->idx_collision == self->head->idx_collision)
                {
                    self->head->next->prev = NULL;
                    self->head = self->head->next;
                    free(node);
                }
                else
                {
                    if(node->collision != NULL)
                    {
                        struct listnode *node_collision = NULL;
                        struct listnode *node_collision_prev = NULL;

                        node_collision_prev = self->hashtab[node->value];
                        node_collision = self->hashtab[node->value]->collision;

                        do
                        {
                            if(node_collision->idx_collision == idx_collision)
                            {
                                if(node_collision->collision != NULL)
                                {
                                    node_collision_prev->collision = node_collision->collision;
                                }
                                free(node_collision);
                            }
                            else
                            {
                                if(node_collision->collision != NULL)
                                {
                                    node_collision = node_collision->collision;
                                }
                                else
                                {
                                    break;
                                }
                            }
                        }
                        while(1);
                    }
                }
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
    int length = sizeof(self->hashtab) / sizeof(*self->hashtab);
    int idx = hash_func(element, length);
    int idx_collision = hash_func_collision(element, length);

    if(self->hashtab[idx]->idx_collision == idx_collision)
    {
        return self->hashtab[idx];
    }
    else
    {
        struct listnode *node_collision = NULL;
        if (self->hashtab[idx]->collision != NULL)
        {
            node_collision = self->hashtab[idx]->collision;
            do
            {
                if(node_collision->idx_collision == idx_collision)
                {
                break;
                }
                else
                {
                    if(node_collision->collision != NULL)
                    {
                        node_collision = node_collision->collision;
                    }
                    else
                    {
                        break;
                    }
                }
            }
            while(1);
            return node_collision;
        }
    }

    return self->hashtab[idx];
}

hashset GetHashTable()
{
    struct hashset ahashset;
     ahashset.head = NULL;
    int length = sizeof(ahashset.hashtab) / sizeof(*ahashset.hashtab);
    for(int i = 0; i < length; i++)
    {
        ahashset.hashtab[i] = NULL;
    }

    ahashset.add = &addElement;
    ahashset.remove_ = &removeElement;
    ahashset.contains = &foundElement;
    return ahashset;
}
