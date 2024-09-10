#include "SortedList.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sched.h>

/*
Global
*/
int opt_yield = 0x0;

void SortedList_insert(SortedList_t *List, SortedListElement_t *anItem) {
  SortedList_t* tNode;
  int elemCheck;
  SortedList_t* pToNode; 
  int caseCheck = 0x0;

  //Check if the List is null to begin with
  if (List == NULL)
    return;
  //Check if the anItem is Null as well
  if(anItem == NULL)
    return;

  //Lastly check if the next anItem in the List is Null, before schdule yield
  if (List->next == NULL) {
  
    // Handle Critical Section
    if (opt_yield & INSERT_YIELD) {
      sched_yield();
    }
    List->next = anItem;
    anItem->prev = List;
    anItem->next = NULL;
    return;
  }

  //Compare the anItems for mismatched values while no null
  //To move through the List
  tNode = List->next; pToNode = List;
  elemCheck = strcmp(anItem->key, tNode->key);
  while ( elemCheck >= 0x0 && tNode != NULL){
    pToNode = tNode;
    tNode = tNode->next;
  } 
  
  //  Handle opt and insert for scheduling
  if (opt_yield & INSERT_YIELD) {
    sched_yield();
  }

  //Two cases depending on position in the List to insert:
  //If our current pointer is null then we ser the pointer to
  //next anItem. Adjust the previous to point to that pointer
  //And set the next to be Null
  if ( tNode == NULL) {caseCheck = 0x1;}
  switch(caseCheck)
    {
    case 0x1:
      pToNode->next = anItem;
      anItem->prev = pToNode;
      anItem->next = NULL;
      break;
      //If the pointer to next is an anItem and the previous is also an
      //anItem then we adjust the anItem to point appropriately to the next
      //and previous
    default:
      pToNode->next = anItem;
      tNode->prev = anItem;
      anItem->next = tNode;
      anItem->prev = pToNode;
    }

}


SortedListElement_t *SortedList_lookup(SortedList_t *List, const char *key) {
  int retVal;
  int checkYL = 0;
  if (List == NULL) {
    retVal = 1;
    if (retVal) {return NULL;}
  }
  //Check if List is empty
  SortedList_t* pToNode = List->next;
  while (pToNode != NULL) {
    // Set check value for scheduling
    checkYL = (opt_yield & LOOKUP_YIELD);
      if(checkYL) {
      sched_yield();
    }
      // if tthe compared values are exactly the same
    if (strcmp(key, pToNode->key) == 0x0) {
      return pToNode;
    }
    pToNode = pToNode->next;
  }
  return NULL;
}

int SortedList_length(SortedList_t *List) {
  int retVal, count;
  count = retVal = 0;
  retVal = 0;
  if (List == NULL) {
    retVal = -1;
    return retVal;
  }
  //  if (List->next == List)
  //    return 0 ;
  SortedList_t* pToNode = List->next;
  while (pToNode != NULL) {

    if (opt_yield & LOOKUP_YIELD) {
      sched_yield();
    }
    pToNode = pToNode->next;
    count++;
  }
  retVal = count;
  return retVal;
}

/*
Used for testing delete cases.
*/
/*
void testD(const char * msg, int num){
  printf("Test in Delete: %s , %d\n", msg, num);
}
*/


int SortedList_delete(SortedListElement_t *anItem) {
  int retVal; //, testVal;
  retVal = 0x0; // = testVal = 0x0
  if (anItem == NULL) {
    retVal = 0x1;
    return retVal;
	}
  else {retVal = 0x0;}
  //Handle Critical Section for delete
    	if (opt_yield & DELETE_YIELD) {
        	sched_yield();
    	}
	//If the next anItem is not NULL then we can still traverse List
	// Laso check if the pointer to prev is not the same elem
	//If so then return 0x1

	if (anItem->next != NULL)
	  {
        	if (anItem->next->prev != anItem)
		  {
		    retVal = 0x1;
		    //		    testVal = 0x1;
		    // testD("Case 1: %d", testVal);
		    return retVal;
		  }
		else
		  {
		    //testVal = 0x2;
		    anItem->next->prev = anItem->prev;
		    //testD("Case 2: %d", testVal);
		  }
	  }
    	if (anItem->prev != NULL)
	  {
	    if (anItem->prev->next != anItem)
	      {
		//testVal = 0x3;
		//testD("Case 3: %d", testVal);
		retVal = 0x1;
		return retVal;
	      }
	    else
	      {
		//testVal = 0x4;
		//testD("Case 4: %d", testVal);
		anItem->prev->next = anItem->next;
	      }
	  }
	retVal = 0x0 ;
	return retVal;
}
