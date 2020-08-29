#if !defined IDCONTAINER_H
#define IDCONTAINER_H

class IDContainer {

  public:
    IDContainer() = default;
    ~IDContainer() = default;

    /*  Extract value from the list
     *  Pre-condition: list is not empty
     */
    int pop(){
      int value = list_head_->value;
      LinkedList* next_val = list_head_->next;
      delete(list_head_);
      list_head_ = next_val;

      return value;
    }
    void push(int const id){

      // Create new element and add it to the top of the list
      LinkedList* new_id = new LinkedList();
      new_id->value = id;
      new_id->next = list_head_;
      list_head_ = new_id;
    }
    bool empty(){

      if(list_head_ == nullptr) return true;
      else return false;
      
    }
    bool find(int const id){

      bool found = false;
      
      LinkedList* current = list_head_;

      while(current != nullptr){
        if(current->value == id){
          found = true;
          break;
        }
        current = current->next;
      }

      return found;
    }

  private:

    struct LinkedList{
      LinkedList* next;
      int value;
    };

    LinkedList* list_head_ = nullptr;
  
};

#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
