#include "stdio.h"

struct List{
    int data;
    List *next;

    List &operator[](int index){
        List *p = this;
        for(int i = 0; i < index; i++){
            p = p->next;
        }
        return *p;
    }
};


struct SudoKu{
    List *list;
    int pos;
}
