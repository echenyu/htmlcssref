#ifndef HEAP_PRIORITY_QUEUE_H
#define HEAP_PRIORITY_QUEUE_H

#include "eecs281_priority_queue.h"
#include <iostream>
//A specialized version of the 'priority_queue' ADT implemented as a binary priority_queue.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class heap_priority_queue : public eecs281_priority_queue<TYPE, COMP_FUNCTOR> {
public:
  typedef unsigned size_type;

  //Description: Construct a priority_queue out of an iterator range with an optional
  //             comparison functor.
  //Runtime: O(n) where n is number of elements in range.
  template<typename InputIterator>
  heap_priority_queue(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR());

  //Description: Construct an empty priority_queue with an optional comparison functor.
  //Runtime: O(1)
  heap_priority_queue(COMP_FUNCTOR comp = COMP_FUNCTOR());

  //Description: Assumes that all elements inside the priority_queue are out of order and
  //             'rebuilds' the priority_queue by fixing the priority_queue invariant.
  //Runtime: O(n)
  virtual void fix();

  //Description: Add a new element to the priority_queue.
  //Runtime: O(log(n))
  virtual void push(const TYPE& val);

  //Description: Remove the most extreme (defined by 'compare') element from
  //             the priority_queue.
  //Note: We will not run tests on your code that would require it to pop an
  //element when the priority_queue is empty. Though you are welcome to if you are
  //familiar with them, you do not need to use exceptions in this project.
  //Runtime: O(log(n))
  virtual void pop();

  //Description: Return the most extreme (defined by 'compare') element of
  //             the priority_queue.
  //Runtime: O(1)
  virtual const TYPE& top() const;

  //Description: Get the number of elements in the priority_queue.
  //Runtime: O(1)
  virtual size_type size() const
    { return data.size()-1;
    /*** Fill this in - might be very simple depending on implementation ***/ }

  //Description: Return true if the priority_queue is empty.
  //Runtime: O(1)
  virtual bool empty() const
    { if(data.size() == 1 || data.size() == 0) {
        return true;
        }
        return false;
        /*** Fill this in - might be very simple depending on implementation ***/ }
private:
  //Note: This vector *must* be used your priority_queue implementation.
  std::vector<TYPE> data;
private:
  //***Add any additional member functions or data you require here.
  void swap (TYPE &a, TYPE &b) {
    TYPE temp = a;
    a = b;
    b = temp; 
  }

  void fixDown(int indexOfParent) {
    while(2 * indexOfParent <= data.size()-1) {
      int j = 2*indexOfParent;
      if(j < data.size()-1 && this->compare(data[j], data[j+1])) {
        j++;
      }
      if(this->compare(data[j], data[indexOfParent])) {
        break;
      }
      swap(data[j], data[indexOfParent]);
      indexOfParent = j; 
    }
  }

  void fixUp(int lowestIndex) {
    while((this->compare(data[lowestIndex/2], data[lowestIndex]) && lowestIndex>1)) {
      swap(data[lowestIndex], data[lowestIndex/2]);
      lowestIndex = lowestIndex/2; 
    }
  }
};

template<typename TYPE, typename COMP_FUNCTOR>
template<typename InputIterator>
heap_priority_queue<TYPE, COMP_FUNCTOR>::heap_priority_queue(
  InputIterator start,
  InputIterator end,
  COMP_FUNCTOR comp
) {
  //Your code.
  this->compare = comp; 
  if(empty()) {
    data.push_back(*start); //This way we can start at index 1; 
  }
  for(InputIterator realStart = start;  realStart != end; realStart++) {
    data.push_back(*realStart);
  }
  //This index has the first parents
  fix(); 
  
}

template<typename TYPE, typename COMP_FUNCTOR>
heap_priority_queue<TYPE, COMP_FUNCTOR>::heap_priority_queue(COMP_FUNCTOR comp) {
  //Your code.
  this->compare = comp; 
}

template<typename TYPE, typename COMP_FUNCTOR>
void heap_priority_queue<TYPE, COMP_FUNCTOR>::fix() {
  //Your code.
  int treeWithChildren = (data.size()-1)/2; 
  while(treeWithChildren >= 1) {
    fixDown(treeWithChildren);
    treeWithChildren--;
  }
}

template<typename TYPE, typename COMP_FUNCTOR>
void heap_priority_queue<TYPE, COMP_FUNCTOR>::push(const TYPE& val) {
  //Your code.
  if(empty()) {
    data.push_back(val); 
  }
  data.push_back(val);
  int lowestIndex = data.size()-1; 
  fixUp(lowestIndex); 
}

template<typename TYPE, typename COMP_FUNCTOR>
void heap_priority_queue<TYPE, COMP_FUNCTOR>::pop() {
  //Your code.
  data[1] = data[data.size()-1];
  data.resize(data.size()-1);
  fixDown(1);
}

template<typename TYPE, typename COMP_FUNCTOR>
const TYPE& heap_priority_queue<TYPE, COMP_FUNCTOR>::top() const {
  //Your code.
  return data[1]; //This line present only so that this provided file compiles.
}

#endif //HEAP_PRIORITY_QUEUE_H

