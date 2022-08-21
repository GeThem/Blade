#include "List.h"

void ListRemoveCurrent(VList& self)
{
	if (self.curr == NULL)
		return;
	VLElem* curr = self.curr;
	if (curr == self.head)
	{
		self.head = curr->next;
	}
	if (curr == self.tail)
	{
		self.tail = curr->prev;
	}
	if (curr->prev != NULL)
		curr->prev->next = curr->next;
	if (curr->next != NULL)
		curr->next->prev = curr->prev;
	self.curr = curr->next;
	free(curr);
}

void ListAppend(VList& self, const VanishText& elem)
{
	VLElem* next = (VLElem*)malloc(sizeof(VLElem));
	if (next == NULL)
		return;
	next->val = elem;
	next->next = next->prev = NULL;
	if (self.head == NULL)
	{
		self.curr = self.head = self.tail = next;
	}
	else
	{
		self.tail->next = next;
		next->prev = self.tail;
		self.tail = next;
	}
}

VLElem* ListNext(VList& self)
{
	if (self.curr->next == NULL)
	{
		self.curr = self.head;
		return NULL;
	}
	self.curr = self.curr->next;
	return self.curr;
}

void ListClear(VList& self)
{
	for (; self.head; ListRemoveCurrent(self));
}