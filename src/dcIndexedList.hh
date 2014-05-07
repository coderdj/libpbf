#ifndef _DCINDEXEDLIST_HH_
#define _DCINDEXEDLIST_HH_

// ****************************************************************
// 
// Name     : dcIndexedList.hh
// Author   : Daniel Coderre, LHEP, Uni Bern
// Date     : 06.05.2014
// Brief    : General linked list class with added bonus that each 
//            item also has a unique handle through which it can be
//            accessed with O(1) speed. Iteration through the list
//            (O(n)) is supported. Inserts and deletes are O(1)
//     
// **************************************************************** 

#include <vector>
#include <stdexcept>

template <typename T>
struct list_element_t
{ 
   list_element_t *prev;
   list_element_t *next;

   bool bFilled;
   
   T* data;
   int handle;
};

template <typename T> 
class dcIndexedList
{
 public:
   dcIndexedList<T>(){
      Initialize();
   };
   dcIndexedList<T>(int size){
      Initialize();
      resize(size);
   };
   virtual ~dcIndexedList(){
      clear();
   };
   
   // copy constructor
   dcIndexedList(const dcIndexedList &obj);
   
   //assignment
   dcIndexedList & operator = (const dcIndexedList &rhs){
      dcIndexedList tmp(rhs);
      tmp.swap(*this);
      return *this;
   };   

   //element access
   T & operator [] (unsigned int i)  {
      if(i<storageVec.size() && storageVec[i].bFilled)
	return *storageVec[i].data;
      throw std::range_error("Index out of range");
   };
  
   
   // int size()
   //   : return size of container
   // 
   inline unsigned int size()  {
      return storageVec.size();
   };
   
   // void resize(int newsize)
   //   : resize container. if size is bigger, simply append. if smaller
   //     remove elements from end of INDEX vec first (may truncate data
   //     but will not break linkage)
   void resize(unsigned int newsize);
   
   // int insert(T data)
   //  : Put data into the list. Returns a handle that allows O(1) access
   //    If no handles are available returns -1
   int insert(T data);
   
   // erase(int handle)
   //  : Remove the data at handle
   void erase(int handle);

   // list_element_t* first()
   //   : returns a pointer to the first filled element 
   inline list_element_t <T>* first(){
      if(storageVec[0].bFilled)
	return storageVec[0];
      return NULL;
   };

   // list_element_t* end()
   //  : return a  pointer to the last filled element
   inline list_element_t <T>* end()  {
      if(lastFilled!=NULL) return lastFilled->next;
      return NULL;
   };
   
   // void clear()
   //  : clear the container
   void clear(){
      for(unsigned int x=0;x<storageVec.size();x++)
	delete storageVec[x].data;
      storageVec.clear();
      firstEl=NULL;
      lastEl=NULL;
      lastFilled=NULL;
   };

 private:
   void Initialize(){
      firstEl=NULL;
      lastEl=NULL;
      lastFilled=NULL;
   };
   
   std::vector < list_element_t <T> > storageVec;

   list_element_t <T>* firstEl;
   list_element_t <T>* lastEl;
   list_element_t <T>* lastFilled;
};


template <typename T>
dcIndexedList<T>::dcIndexedList(const dcIndexedList &obj)
{   
   obj.storageVec.resize(storageVec.size());
   for(unsigned int x=0;x<storageVec.size();x++)    {      
      if(storageVec[x].prev!=NULL)
	obj.storageVec[x].prev = &obj.storageVec[storageVec[x].prev->handle];
      else
	obj.storageVec[x].prev=NULL;
      if(storageVec[x].next!=NULL)
	obj.storageVec[x].next = &obj.storageVec[storageVec[x].next->handle];
      else
	obj.storageVec[x].next = NULL;
      obj.storageVec[x].handle = storageVec[x].handle;
      obj.storageVec[x].bFilled = storageVec[x].bFilled;
      obj.storageVec[x].data = new T(obj.storageVec[x].data);
   }
      
   if(firstEl==NULL)    obj.firstEl=NULL;
   else   obj.firstEl = obj.storageVec[firstEl->handle];
   if(lastEl==NULL)     obj.lastEl=NULL;
   else    obj.lastEl = obj.storageVec[lastEl->handle];
   if(lastFilled==NULL) obj.lastFilled=NULL;
   else    obj.lastFilled = obj.storageVec[lastFilled->handle];
}

template <typename T>
void dcIndexedList<T>::resize(unsigned int newsize) 
{   
   if(newsize == size()) return; //nothing to do
   if(newsize>size())   {	
      //make containers larger
      int startsize = storageVec.size();
      storageVec.resize(newsize);      
      
      //assign handles for direct access. these are unique ints
      //also set prev and next pointers for each element
      for(unsigned int x=startsize;x<storageVec.size();x++)     {	     
	 storageVec[x].handle=x;
	 storageVec[x].bFilled=false;
	 storageVec[x].data=NULL;
	 if(x+1!=storageVec.size())
	   storageVec[x].next=&storageVec[x+1];
	 else storageVec[x].next=NULL; //only last element has NULL next pointer 
	 if(x!=0)
	   storageVec[x].prev = &storageVec[x-1]; //possibly overwritten below
	 else storageVec[x].prev=NULL; //only first element has NULL prev pointer
      }
      
      if(startsize==0) firstEl = &storageVec[0];
      if(lastEl!=NULL)   {	 
	 lastEl->next = &storageVec[startsize];
	 storageVec[startsize].prev = lastEl;
      }      
      lastEl = &storageVec[storageVec.size()-1];
      return;
   }
     // else if size<size()
   while(storageVec.size()>newsize)  {
      erase(storageVec.size()-1); //removes element to end of list and resets poi$
      lastEl = lastEl->prev;
      lastEl->next=NULL;
      storageVec.resize(storageVec.size()-1);
   }
   return;
}

template <typename T>
int dcIndexedList<T>::insert(T data)
{   
   if(lastFilled==NULL)       {	
      //this is the first element, return index 0
      if(storageVec[0].bFilled) return -1;
      storageVec[0].data = new T(data);
      storageVec[0].bFilled=true;
      return 0;
   }   
   if(lastFilled->next==NULL || lastFilled->next->bFilled) return -1;
   lastFilled = lastFilled->next;
   lastFilled->bFilled=true;
   if(lastFilled->data!=NULL) delete lastFilled->data;
   lastFilled->data=new T(data);
   return lastFilled->handle;
}

template <typename T>
void dcIndexedList<T>::erase(int handle)
{   
   //puts element to end of list
   
   //set private pointers to proper values
   if(lastFilled!=NULL && storageVec[handle].handle == lastFilled->handle)
     lastFilled = lastFilled->prev;
   if(lastEl!=NULL && storageVec[handle].handle == lastEl->handle)
     lastEl = lastEl->prev;
   if(firstEl!=NULL && storageVec[handle].handle== firstEl->handle)
     firstEl = firstEl->next;
   
   //set previous/next element pointers to proper values
   if(storageVec[handle].prev!=NULL)
             storageVec[handle].prev->next = storageVec[handle].next;
   if(storageVec[handle].next!=NULL)
     storageVec[handle].next->prev = storageVec[handle].prev;
   
   //move to end of array
   lastEl->next = &storageVec[handle];
   storageVec[handle].prev = lastEl;
   storageVec[handle].next = NULL;
   
   //set this el to last el
   lastEl = &storageVec[handle];
   
   //clear data
   lastEl->bFilled=false;
   delete lastEl->data;
   lastEl->data=NULL;
}

   
   
#endif
