#include <stdio.h>
#include <stdlib.h>
#include "hash_set.h"


int main()
{
    test();
    printf("Hello world!\n");

    struct hashset hashset_ = GetHashTable();

    hashset_.add(&hashset_,"ivanov");
    hashset_.add(&hashset_,"petrov");
    hashset_.add(&hashset_,"petrov");
    hashset_.add(&hashset_,"sidorov");
    hashset_.add(&hashset_,"sidorov");
    hashset_.add(&hashset_,"sidorov");
    hashset_.add(&hashset_,"sidorov");

    for(listnode *hs = hashset_.head; hs != NULL; hs = hs->next)
    {
        printf("%d - ",hs->value);
        printf("%s - ",hs->key);
        printf("%d\n",hs->count_);
    }

    return 0;
}
