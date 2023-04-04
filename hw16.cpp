#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <vector>
#include <iterator>
#include <functional>
#include <numeric>
#include <set>
#include <map>
#include <sstream>
#include <cctype>
using std::isalpha;
using std::cout;
using std::endl;
using std::cin;
using std::ostream_iterator;
using std::string;
using std::function;
using std::vector;
using std::remove_copy_if;
using std::accumulate;
using std::sort;
using std::any_of;
using std::copy_if;
using std::transform;
using std::to_string;
using std::reverse;
using std::remove_if;
using std::ofstream;
using std::ifstream;
using std::ostream;
using std::set;
using std::map;
using std::istream;
using std::istringstream;
using std::tolower;
using std::remove_copy;

string to_lower_case(string a){
    string b;
    for(size_t i = 0; i < a.size(); i++){
        b += tolower(a.at(i));
    }
    return b;
}

string replace_str(istream &text_in, ostream &out1, map<string,string> sts){
    string each_line, low_line;
    set<string> a;  
    getline(text_in, each_line);
    for(size_t i = 0; i < each_line.size(); i++){
      low_line += tolower(each_line[i]);
    }
    for(auto itr = sts.begin(); itr != sts.end(); itr++){
        string key_word = itr->first, replace_word = itr->second;        
        string key_word_low = to_lower_case(key_word);
        int location = low_line.find(key_word_low);
        int key_long = key_word.size();
        while(location != string::npos){
          low_line.erase(low_line.begin() + location, low_line.begin() + location + key_long);
          each_line.erase(each_line.begin() + location, each_line.begin() + location + key_long);
          low_line.insert(location, replace_word);
          each_line.insert(location, replace_word);
          location = low_line.find(key_word_low);
        }                        
    }
    out1 << each_line;      
    return each_line;
}

set<string> ReplacementCensor(istream &text_in, ostream &out1, map<string,string> sts){
    string each_line, low_line;
    set<string> a;  
    getline(text_in, each_line);
    for(size_t i = 0; i < each_line.size(); i++){
      low_line += tolower(each_line[i]);
    }
    for(auto itr = sts.begin(); itr != sts.end(); itr++){
        string key_word = itr->first, replace_word = itr->second;        
        string key_word_low = to_lower_case(key_word);
        auto location = low_line.find(key_word_low);
        int key_long = key_word.size();
        while(location != string::npos){
          string replaced_word;
          low_line.erase(low_line.begin() + location, low_line.begin() + location + key_long);
          for(int i = 0; i < key_long; i++){
            replaced_word += each_line[location + i];
          }
          each_line.erase(each_line.begin() + location, each_line.begin() + location + key_long);
          low_line.insert(location, replace_word);
          each_line.insert(location, replace_word);
          location = low_line.find(key_word_low);
          a.insert(replaced_word);
          location = low_line.find(key_word_low);
        }                        
    }
    out1 << low_line;      
    return a;
}

int main(){
    map<string,string> bad_to_good = {{"word", "Grouped-Letter-Unit"}, {"be", "wasp"}, {"not found", "not appearing"}, {"PlaCe", "LoCation"}};

    std::istringstream iss("note: this is a line with multiple WORds that should be rePLACEd./n all instances of word eveninlargerWordsshould be repLAced.");
    std::ostringstream oss;
    set<string> result = ReplacementCensor(iss, oss, bad_to_good);    
    set<string> expected_return = {"PLACE", "WORd", "Word", "be", "pLAce", "word"};
    if(result == expected_return){
        cout << 1 << endl;
    }
    else{
      cout << 0 << endl;
    }
    // cout << replace_str(iss, oss, bad_to_good);

    return 0;
}

