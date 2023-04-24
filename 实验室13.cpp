#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <set>
using std::set;
using std::string;
using std::ostringstream;
using std::ostream;
using std::vector;
using std::cout;
using std::endl;

struct Node {
    public:
        int data_;
        Node *next_;
   
        Node() : data_(0), next_(nullptr) {};
        Node(int d) : data_(d), next_(nullptr) {};
};

class SingleLink {
    private:
        Node *head_;
        Node *tail_;

    public:
        SingleLink();        
        SingleLink(int dat);    
        void append_back(int);

        friend std::ostream& operator<<(std::ostream &out, const SingleLink &s);
        bool del(int val);
        Node& operator[](size_t index);
       
        // Rule of three stuff
        ~SingleLink();
        SingleLink(const SingleLink &);
        SingleLink& operator=(const SingleLink&);
};

SingleLink::SingleLink(){
    head_ = nullptr;
    tail_ = nullptr;
}  

SingleLink::SingleLink(int dat){
  head_ = nullptr;
  tail_ = nullptr;
  append_back(dat);
}

void SingleLink::append_back(int dat){
    Node *new_node = new Node(dat);
    if(head_ == nullptr){
        head_ = new_node;
        tail_= new_node;
    }
    else{
        tail_->next_ = new_node;
        tail_ = new_node;
    }
}

ostream& operator<<(ostream &out, const SingleLink &s){
    ostringstream store;
    Node *new_ = s.head_;
    while(new_ != nullptr){      
      store << new_->data_;
      new_ = new_->next_;
      if(new_ != nullptr){
        store << ", ";
      }
    }    
    string total = store.str();
    out << total;
    return out;
}

bool SingleLink::del(int val){
    Node *new_ = head_;
    Node *sec_new = nullptr;
    while(new_ != nullptr){ 
      if(new_->data_ == val){
        if(sec_new == nullptr){
            head_ = head_->next_;            
        }
        else{
          sec_new->next_ = new_->next_;
        } 
        if(new_->next_ == nullptr){
          tail_ =  sec_new;
        }
        delete new_;
        return true;    
      }     
      sec_new = new_;
      new_ = new_->next_;     
    }
    return false;
}

SingleLink::~SingleLink(){
  Node* new_ = head_, *sec_new = nullptr;
  while (new_ != nullptr) {
      sec_new = new_->next_;
      delete new_;
      new_ = sec_new;
  }  
}

SingleLink::SingleLink(const SingleLink & new_){
    head_ = new_.head_;
    tail_ = new_.tail_;
}

SingleLink& SingleLink::operator=(const SingleLink& new_){
    head_ = new_.head_;
    tail_ = new_.tail_;
    return *this;
}

Node& SingleLink::operator[](size_t index){
  Node* new_ = head_;
  for(size_t i = 0; i < index; i++){
    if(new_ == nullptr){
      delete new_;
      throw std::out_of_range("out_of_range");
    }
    new_ = new_->next_;
  }
  if(new_ == nullptr){
    delete new_;
    throw std::out_of_range("out_of_range");
  }
  return *new_;
}

int main() {
  // std::ostringstream oss;
  // SingleLink sl(10);
  // oss << sl;
  // sl.append_back(22);
  // sl.append_back(33);
  // sl.del(22);
  // std::string s = oss.str();
  // std::string ans = "10";
  // if(s == ans){cout << 1;}

    SingleLink sl(10);
  sl.append_back(22);
  sl.append_back(33);
  bool had_except = false;
  if(sl[2].data_ == 33){cout << 1 << endl;}
  had_except = false;
  try { sl[2]; } catch(...)  { had_except = true; }
  if(had_except == false){cout << 2 << endl;}
  had_except = false;
  try { sl[3]; } catch(...)  { had_except = true; }
  if(had_except == true){cout << 3 << endl;}
  had_except = false;
  try { sl[100]; } catch(...)  { had_except = true; }
  if(had_except == true){cout << 4 << endl;}
  had_except = false;
  try { sl[1]; } catch(...)  { had_except = true; }
  if(had_except == false){cout << 5 << endl;}
  had_except = false;
  try { sl[-1]; } catch(...)  { had_except = true; }
  if(had_except == true){cout << 6 << endl;}
  had_except = false;
  try { sl[0]; } catch(...)  { had_except = true; }
  if(had_except == false){cout << 7 << endl;} 
}
