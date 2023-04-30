#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <numeric>
#include <set>
#include <map>
#include <sstream>
#include <iomanip>
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::remove_if;
using std::ostream;
using std::set;
using std::map;
using std::ostringstream;
using std::find_if;
using std::remove_if;
using std::fixed;
using std::setprecision;

struct Order {
  std::string username;
  std::string side;  // Can be "Buy" or "Sell"
  std::string asset;
  int amount;
  int price;
  friend ostream& operator<<(std::ostream&, const Order&);
  friend bool operator==(const Order&, const Order&);
};

ostream& operator<<(std::ostream& out_, const Order& order_in){
	out_ << order_in.side << ' ' << order_in.amount << ' ' << order_in.asset;
	out_ << " at " << order_in.price << " USD by " << order_in.username;
	return out_;

}

bool operator==(const Order& fi, const Order& se){
	if(fi.username != se.username){return false;}
	if(fi.side != se.side){return false;}
	if(fi.asset != se.asset){return false;}
	if(fi.amount != se.amount){return false;}
	if(fi.price != se.price){return false;}
	return 1;
}

struct Trade {
  std::string buyer_username;
  std::string seller_username;
  std::string asset;
  int amount;
  int price;
};

class Exchange {
	private:
		map<string, float> assets = {{"BTC", 0}, {"ETH", 0}, {"USD", 0}};
		map<string, map<string, float>> user_account;
		vector<Order> open_order;
		vector<string> trade_history;
		map<string, vector<Order>> record_open, record_filled;
	public:
		void MakeDeposit(const std::string &username, const std::string &asset,
						int amount);
		void PrintUserPortfolios(std::ostream &os) const;
		bool MakeWithdrawal(const std::string &username, const std::string &asset, int amount);
		bool AddOrder(const Order &order);
		bool sale(Order order);
		bool buy(Order order);
		// need the copy of order
		string out_(const Order &order);
		void delete_open_record(const Order&);
		void delete_open_order(vector<Order>::iterator);
		vector<Order>::iterator find_low_price(const Order&);
		vector<Order>::iterator find_high_price(const Order&);	
		vector<Order>::iterator find_first_price(const Order&);	
		vector<Order>::iterator find_record(const Order&);
		void PrintUsersOrders(std::ostream &os);	
		void sale_order_more(vector<Order>::iterator&, Order&);
		void sale_order_less(vector<Order>::iterator&, Order&);
		void sale_same(vector<Order>::iterator&, Order&);
		void buy_order_less(vector<Order>::iterator&, Order&);
		void buy_order_more(vector<Order>::iterator&, Order&);
		void buy_same(vector<Order>::iterator&, Order&);
		// need to change the order and iterator
		void PrintTradeHistory(std::ostream &os) const;
		void add_history(const Order&, const Order&);
		void PrintBidAskSpread(std::ostream &os) const;
		int find_low_sell(const string&) const;
		int find_high_buy(const string&) const;
		void check_zero(std::ostream &os, int&) const;
};

void Exchange::delete_open_order(vector<Order>::iterator order){
	open_order.erase(order);
}

void Exchange::delete_open_record(const Order& order){
	auto re = remove_if(record_open[order.username].begin(), record_open[order.username].end(), [order](Order each){
		if(each == order){
			return true;
		}
		return false;
	});
	record_open[order.username].erase(re, record_open[order.username].end());
}

string Exchange::out_(const Order &order){
  // make users' orders as a string
	ostringstream oss;
	oss << order.side << ' ' << order.amount << ' ' << order.asset;
	oss << " at " << order.price << " USD by " << order.username;
	return oss.str();
}

void Exchange::PrintUsersOrders(ostream &os){
	os << "Users Orders (in alphabetical order):\n";
	for(auto i : record_open){
    // loop each order of each person
		os << i.first << "'s Open Orders (in chronological order):\n";
		for(auto a : i.second){
			string record = out_(a);
			os << record << endl;
		}
		os << i.first << "'s Filled Orders (in chronological order):\n";
		for(auto b : record_filled[i.first]){
			string record = out_(b);
			os << record << endl;		}
	}
}

bool Exchange::MakeWithdrawal(const std::string &username, const std::string &asset, int amount){
	if(user_account.count(username) == 0){
		return false;
	}
	if(user_account[username][asset] < amount){
		return false;
	}
	user_account[username][asset] -= amount;
	return true;
}

void Exchange::MakeDeposit(const string& username, const string& asset, int amount){	
	if(user_account.count(username) == 0){
    //add new account
		user_account[username] = assets;
		record_open[username] = {};
		record_filled[username] ={};
	}	
  if(user_account[username].count(asset) == 0){
    //add new asset
    user_account[username][asset] = 0;
  }
	user_account[username][asset] += amount;	
}

void Exchange::PrintUserPortfolios(ostream& os) const{
	os << setprecision(0) << fixed;
  os << "User Portfolios (in alphabetical order):" << endl;
	for(auto i = user_account.begin(); i != user_account.end(); i++){
        os << i->first << "'s Portfolio: ";
		for(auto a = i->second.begin(); a != i->second.end(); a++){
			if(a->second != 0){
				os << a->second << ' ' << a->first << ", ";
			}			
		}
		os << endl;
    }
}

vector<Order>::iterator Exchange::find_low_price(const Order& order){
	int low_price = order.price;
	vector<Order>::iterator result = open_order.begin(), preresult = open_order.end();
  // loop the open order and find the lowest price
	while(result != open_order.end()){
		result = find_if(result, open_order.end(), [order, &low_price](Order each){
			if(each.asset == order.asset && each.side == "Sell"){
				if(each.price <= low_price){
          low_price = each.price;
					return true;
				}					
			}
			return false; 
		});
		if(result == open_order.end()){
			break;}
		preresult = result;		
		result++;
	}
  if(preresult != open_order.end()){
    return find_first_price(*preresult);
  }
	return preresult;
}



vector<Order>::iterator Exchange::find_high_price(const Order& order){
	int high_price = order.price;
	vector<Order>::iterator result = open_order.begin(), preresult = open_order.end();
  // loop the open order and find the highest price
	while(result != open_order.end()){
		result = find_if(result, open_order.end(), [order, &high_price](Order each){
			if(each.asset == order.asset && each.side == "Buy"){
				if(each.price >= high_price){
          high_price = each.price;
					return true;
				}					
			}			
			return false; 
		});
		if(result == open_order.end()){
			break;}
		preresult = result;
		result++;
	}
  if(preresult != open_order.end()){
    return find_first_price(*preresult);
  }  
	return preresult;
}

vector<Order>::iterator Exchange::find_first_price(const Order& order){
  //funtion find_high_price and find_low_price might find the last order
  //which hold the lowest/highest price wihich hold such price
  //this function find the first order
  int price = order.price;
	vector<Order>::iterator result = open_order.begin();
  result = find_if(result, open_order.end(), [order, price](Order each){
			if(each.asset == order.asset && each.side == order.side && each.price == price){
        return true;
      }
			return false; 
		});
  return result;
}

vector<Order>::iterator Exchange::find_record(const Order& order){
	vector<Order>::iterator result = record_open[order.username].begin();
  result = find_if(result, record_open[order.username].end(), [order](Order each){
			if(each== order){
        return true;
      }
			return false; 
		});
  return result;
}

void Exchange::sale_order_more(vector<Order>::iterator& result, Order& order){
  Order market = *result, new_order = order;
  new_order.amount = result->amount;
  market.price = order.price;
  add_history(new_order, market);
  record_filled[market.username].push_back(market);
  record_filled[order.username].push_back(new_order);
  // record filled order
  MakeDeposit(order.username, "USD", order.price * market.amount);
  MakeDeposit(market.username, market.asset, market.amount);
  order.amount -= result->amount;
  delete_open_record(*result);
  delete_open_order(result);
  // delete filled order
}

void Exchange::sale_order_less(vector<Order>::iterator& result, Order& order){
  Order market = *result;
  market.amount = order.amount;
  market.price = order.price;
  add_history(order, market);
  record_filled[order.username].push_back(order);
  record_filled[market.username].push_back(market);
  // record filled order
  MakeDeposit(order.username, "USD", order.price * order.amount);
  MakeDeposit(market.username, market.asset, order.amount);
  vector<Order>::iterator object = find_record(*result);
  object->amount -= order.amount;
  result->amount -= order.amount;
  order.amount = 0;
}

void Exchange::sale_same(vector<Order>::iterator& result, Order& order){
  Order market = *result;
  market.price = order.price;
  add_history(order, market);
  record_filled[order.username].push_back(order);
  record_filled[market.username].push_back(market);
  MakeDeposit(order.username, "USD", order.price * order.amount);
  MakeDeposit(market.username, market.asset, order.amount);
  delete_open_record(*result);
  order.amount = 0;
  delete_open_order(result);
}

bool Exchange::sale(Order order){
	bool option = MakeWithdrawal(order.username, order.asset, order.amount);
  if(!option){
    return false;
  }
  // loop all order untile there are not appropriate buy order
  while(order.amount != 0){
    vector<Order>::iterator result = find_high_price(order);
    if(result == open_order.end()){		
      open_order.push_back(order);
      record_open[order.username].push_back(order);
      break;
    }
    if(result->amount < order.amount){
      sale_order_more(result, order);
    }
    else if(result->amount > order.amount){
      sale_order_less(result, order);
    }
    else if(result->amount == order.amount){
      sale_same(result, order);
    }
  }
	return option;	
}

void Exchange::buy_order_less(vector<Order>::iterator& result, Order& order){
  Order market = *result;
  market.amount = order.amount;
  market.price = order.price;
  add_history(market, order);
  record_filled[order.username].push_back(order);
  record_filled[market.username].push_back(market);
  MakeDeposit(result->username, "USD", order.price * order.amount);
  MakeDeposit(order.username, order.asset, order.amount);
  vector<Order>::iterator object = find_record(*result);
  object->amount -= order.amount;
  result->amount -= order.amount;  
  order.amount = 0;
}

void Exchange::buy_order_more(vector<Order>::iterator& result, Order& order){
  Order market = *result, new_order = order;
  new_order.amount = result->amount;
  market.price = order.price;
  add_history(market, new_order);
  record_filled[market.username].push_back(market);
  record_filled[order.username].push_back(new_order);
  MakeDeposit(result->username, "USD", order.price * result->amount);
  MakeDeposit(order.username, order.asset, market.amount);
  order.amount -= result->amount;
  delete_open_record(*result);
  delete_open_order(result);
}
void Exchange::buy_same(vector<Order>::iterator& result, Order& order){
  Order market = *result;
  market.price = order.price;
  add_history(market, order);
  record_filled[order.username].push_back(order);
  record_filled[market.username].push_back(market);
  MakeDeposit(result->username, "USD", order.price * order.amount);
  MakeDeposit(order.username, order.asset, order.amount);
  delete_open_record(*result);
  order.amount = 0;
  delete_open_order(result);
}

bool Exchange::buy(Order order){
	bool option = MakeWithdrawal(order.username, "USD", order.amount * order.price);
  if(!option){
    return false;
  }
  // loop all order untile there are not appropriate sale order
  while(order.amount != 0){
    vector<Order>::iterator result = find_low_price(order);
    if(result == open_order.end()){		
      open_order.push_back(order);
      record_open[order.username].push_back(order);
      break;
    }
    if(result->amount < order.amount){
      buy_order_more(result, order);
    }
    else if(result->amount > order.amount){
      buy_order_less(result, order);
    }
    else if(result->amount == order.amount){
      buy_same(result, order);
    }
  }
	return option;	
}

bool Exchange::AddOrder(const Order &order){	
	if(order.side == "Sell"){
		return sale(order);
	}
	return buy(order);
}




void Exchange::add_history(const Order& saller, const Order& buyer){  
  ostringstream oss;
  oss << buyer.username << " Bought " << buyer.amount << " of ";
  oss << buyer.asset << " From " << saller.username << " for "<< buyer.price;
  oss << " USD\n";
  trade_history.push_back(oss.str());
}

void Exchange::PrintTradeHistory(std::ostream &os) const{
  os << "Trade History (in chronological order):\n";
  for(auto i : trade_history){
    os << i;
  }
}

int Exchange::find_low_sell(const string& asset) const{
  int lowest_price = 2147483647;
  //max int value
  //loop for lowest price of such asset from sell order
  for(auto i : open_order){
    if(i.price < lowest_price && i.side == "Sell" && i.asset == asset){
        lowest_price = i.price;
      }		
  }
  if(lowest_price == 2147483647){
    return 0;
  }
	return lowest_price;
}

int Exchange::find_high_buy(const string& asset) const{
  int high_price = 0;
  //loop for highest price of such asset from buy order
  for(auto i : open_order){
    if(i.price > high_price && i.side == "Buy" && i.asset == asset){
        high_price = i.price;
      }		
  }
	return high_price;
}

void Exchange::check_zero(std::ostream &os, int& price) const{
  if(price == 0){
      os << "NA";
    }
    else {
      os << price;
    }
}

void Exchange::PrintBidAskSpread(std::ostream &os) const{
  set<string> asset = {};
  for(auto i : open_order){
    asset.insert(i.asset);
  }
  os << "Asset Bid Ask Spread (in alphabetical order):\n";
  for(string i : asset){
    int price_high = find_high_buy(i), price_low = find_low_sell(i);    
    os << i << ": Highest Open Buy = ";
    check_zero(os, price_high);
    os << " USD and Lowest Open Sell = ";
    check_zero(os, price_low);
    os << " USD\n";
  }
}




int main(){
	Exchange e;
  std::ostringstream oss;
  e.MakeDeposit("Nahum", "BTC", 1000);
  e.MakeDeposit("Nahum", "USD", 100000);
  e.MakeDeposit("Dolson", "USD", 555555);
  e.MakeDeposit("Ofria", "ETH", 678);
  e.MakeDeposit("Zaabar", "USD", 12121212);
  e.MakeDeposit("Zaabar", "LTC", 4563);
  // BTC rough price 1000
  // ETH rough price 100
  // LTC rough price 500
  e.AddOrder({"Nahum", "Sell", "BTC", 5, 1100});
  e.AddOrder({"Nahum", "Sell", "BTC", 100, 1200});
  e.AddOrder({"Nahum", "Buy", "BTC", 7, 800});
  e.AddOrder({"Dolson", "Buy", "BTC", 1, 950});
  e.AddOrder({"Ofria", "Sell", "ETH", 12, 156});
  e.AddOrder({"Ofria", "Sell", "ETH", 10, 160});
  e.AddOrder({"Zaabar", "Sell", "LTC", 10, 550});
  e.AddOrder({"Zaabar", "Buy", "LTC", 10, 450});
  e.AddOrder({"Nahum", "Buy", "LTC", 55, 600});
  e.AddOrder({"Nahum", "Buy", "ETH", 30, 158});
  e.AddOrder({"Ofria", "Sell", "ETH", 10, 140});
  e.AddOrder({"Zaabar", "Buy", "BTC", 10, 1500});
  e.AddOrder({"Zaabar", "Buy", "ETH", 200, 1200});
  e.AddOrder({"Nahum", "Sell", "BTC", 30, 158});
  e.AddOrder({"Dolson", "Buy", "BTC", 20, 2000});
  e.AddOrder({"Nahum", "Sell", "ETH", 1, 1423});
  e.PrintUserPortfolios(std::cout);
  oss.str("");
  e.PrintUserPortfolios(oss);
  if(oss.str() == "User Portfolios (in alphabetical order):\nDolson's Portfolio: 21 BTC, 514605 USD, \nNahum's Portfolio: 872 BTC, 21 ETH, 10 LTC, 112924 USD, \nOfria's Portfolio: 646 ETH, 15296 USD, \nZaabar's Portfolio: 10 BTC, 10 ETH, 4553 LTC, 11867712 USD, \n"){cout << 1 << endl;}
  e.PrintUsersOrders(std::cout);
  oss.str("");
  e.PrintUsersOrders(oss);
  if(oss.str() == "Users Orders (in alphabetical order):\nDolson's Open Orders (in chronological order):\nDolson's Filled Orders (in chronological order):\nBuy 1 BTC at 158 USD by Dolson\nBuy 20 BTC at 2000 USD by Dolson\nNahum's Open Orders (in chronological order):\nSell 95 BTC at 1200 USD by Nahum\nBuy 45 LTC at 600 USD by Nahum\nBuy 8 ETH at 158 USD by Nahum\nSell 2 BTC at 158 USD by Nahum\nSell 1 ETH at 1423 USD by Nahum\nNahum's Filled Orders (in chronological order):\nBuy 10 LTC at 600 USD by Nahum\nBuy 12 ETH at 158 USD by Nahum\nBuy 10 ETH at 140 USD by Nahum\nSell 5 BTC at 1500 USD by Nahum\nSell 5 BTC at 1500 USD by Nahum\nSell 1 BTC at 158 USD by Nahum\nBuy 7 BTC at 158 USD by Nahum\nSell 7 BTC at 158 USD by Nahum\nSell 20 BTC at 2000 USD by Nahum\nOfria's Open Orders (in chronological order):\nOfria's Filled Orders (in chronological order):\nSell 12 ETH at 158 USD by Ofria\nSell 10 ETH at 140 USD by Ofria\nSell 10 ETH at 1200 USD by Ofria\nZaabar's Open Orders (in chronological order):\nBuy 10 LTC at 450 USD by Zaabar\nBuy 190 ETH at 1200 USD by Zaabar\nZaabar's Filled Orders (in chronological order):\nSell 10 LTC at 600 USD by Zaabar\nBuy 5 BTC at 1500 USD by Zaabar\nBuy 5 BTC at 1500 USD by Zaabar\nBuy 10 ETH at 1200 USD by Zaabar\n"){cout << 2 << endl;}
  std::cout << std::endl << std::endl;
	return 0;
}
