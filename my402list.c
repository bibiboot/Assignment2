#include <stdio.h>
#include <stdlib.h>
#include "my402list.h"

/**
 * Initialize the list.
 * @author Danish Rehman
 * @param list pointer.
 */
int My402ListInit(My402List *myList)
{
    /*Initialize the list*/
    myList->anchor.next = &(myList->anchor);
    myList->anchor.prev = &(myList->anchor);
    myList->anchor.obj = NULL;
    myList->num_members = 0;
    return TRUE;
}

/**
 * Initialize the list.
 * @author Danish Rehman
 * @param list pointer.
 */
int  My402ListLength(My402List *myList)
{
    return myList->num_members;
}

/**
 * Initialize the list.
 * @author Danish Rehman
 * @param list pointer.
 */
int  My402ListEmpty(My402List *myList)
{
    if (myList->num_members == 0){
        return TRUE;
    }
    else {
        return FALSE;
    }
}
 
/**
 * Initialize the list.
 * @author Danish Rehman
 * @param list pointer.
 */
int My402ListAppend(My402List *myList, void* dest)
{
    struct tagMy402ListElem *temp;
    My402ListElem *new_node = malloc(sizeof(My402ListElem));
    temp = myList->anchor.prev;
    new_node->prev = temp;
    temp->next = new_node;
    new_node->next = &(myList->anchor);
    new_node->obj = dest;
    myList->anchor.prev = new_node;
    myList->num_members++;

    return TRUE;
}

/**
 * Initialize the list.
 * @author Danish Rehman
 * @param list pointer.
 */
int  My402ListPrepend(My402List *myList, void* dest)
{
    struct tagMy402ListElem *temp;
    My402ListElem *new_node = malloc(sizeof(My402ListElem));
    temp = myList->anchor.next;
    new_node->next = temp;
    new_node->prev = &(myList->anchor);
    new_node->obj = dest;
    myList->anchor.next = new_node;
    temp->prev = new_node;
    myList->num_members++;

    return TRUE;
}

/**
 * Initialize the list.
 * @author Danish Rehman
 * @param list pointer.
 */
void My402ListUnlink(My402List *myList, My402ListElem *dest)
{
    (dest->prev)->next = dest->next;
    (dest->next)->prev = dest->prev;
    myList->num_members--;
    free(dest);
}

/**
 * Initialize the list.
 * @author Danish Rehman
 * @param list pointer.
 */
void My402ListUnlinkAll(My402List *myList)
{
    My402ListElem *elem=NULL;
    My402ListElem *last_elem=NULL;

    for (elem=My402ListFirst(myList);elem != NULL; elem=My402ListNext(myList, elem)) {
        (myList->anchor).next = elem->next;
        (elem->next)->prev = &(myList->anchor);
        free(last_elem);
        last_elem = elem;
        myList->num_members--;
    }
    free(last_elem);
}

/**
 * Initialize the list.
 * @author Danish Rehman
 * @param list pointer.
 */
int My402ListInsertBefore(My402List *myList, void *obj, My402ListElem *elem)
{
    if (elem == NULL)
        return My402ListPrepend(myList, obj);
    My402ListElem *new_node = malloc(sizeof(My402ListElem));
    new_node->obj = obj;
    (elem->prev)->next = new_node;
    new_node->prev = elem->prev;
    new_node->next = elem;
    elem->prev = new_node;
    myList->num_members++;
    return TRUE;
}

/**
 * Initialize the list.
 * @author Danish Rehman
 * @param list pointer.
 */
int My402ListInsertAfter(My402List *myList, void *obj, My402ListElem *elem)
{
   if (elem == NULL)
        return My402ListAppend(myList, obj);
    My402ListElem *new_node = malloc(sizeof(My402ListElem));
    new_node->obj = obj;
    (elem->next)->prev = new_node;
    new_node->next = elem->next;
    new_node->prev = elem;
    elem->next = new_node;
    myList->num_members++;
    return TRUE;
}

/**
 * Initialize the list.
 * @author Danish Rehman
 * @param list pointer.
 */
My402ListElem* My402ListFirst(My402List *myList)
{
    if (myList->num_members == 0)
        return NULL;
    return myList->anchor.next;

}

/**
 * Initialize the list.
 * @author Danish Rehman
 * @param list pointer.
 */
My402ListElem* My402ListLast(My402List *myList)
{
    if (myList->num_members == 0)
        return NULL;
    return myList->anchor.prev;

}

/**
 * Initialize the list.
 * @author Danish Rehman
 * @param list pointer.
 */
My402ListElem* My402ListNext(My402List *myList, My402ListElem *dest)
{
    if (dest->next == &(myList->anchor))
        return NULL;
    return dest->next;
}

/**
 * Get the previous element of the 
 * dest pointer element.
 * @author Danish Rehman
 * @param main pointer.
 * @param dest pointer relative to which previous 
          is to be found.
 * @return NULL if dest is the first element of list.
 *         previous element otherwise.
 */
My402ListElem* My402ListPrev(My402List *myList, My402ListElem *dest)
{
    if (dest->prev == &(myList->anchor))
        return NULL;
    return dest->prev;
}

/**
 * Find the obj in the list.
 * @author Danish Rehman
 * @param main pointer.
 * @param obj pointer to search for.
 * @return NULL in case of not found.
 *         Element in case of found.
 */
My402ListElem* My402ListFind(My402List *myList, void *obj){
    My402ListElem *elem=NULL;

    for (elem=My402ListFirst(myList);elem != NULL; elem=My402ListNext(myList, elem)) {
        if (obj == elem->obj)
            return elem;
    }
    return NULL;
}
