//Name: Eric Yu
//Class: EECS 281
//Description: This project uses various data structures to create and
//simulate a stock market. Utilizes hash tables, and priority queues

#include <iostream>
#include <getopt.h>
#include <sstream>
#include <string>
#include <unordered_map> 
#include <queue> 
#include <vector>
#include <random> 
#include "medianDataStructure.h"
#include <limits.h>

using namespace std;

struct clientStocks {
	long long int numberBought;
	long long int numberSold;
	long long int netProfit;
};

struct orderStruct {
	long long int idNumber; 
	long long int price;
	long long int quantity;
	string clientName;
};

struct ttStruct {
	long long int lowestBuy;
	long long int highestSell;
	long long int timestamp1;
	long long int timestamp2; 
	long long int order1;
	long long int order2; 
	long long int ttStamp1;
	long long int ttStamp2; 
	long long int maxProfit;
};

struct buyComparator {
	bool operator() (orderStruct a, orderStruct b) {
		if(a.price == b.price) {
			return a.idNumber > b.idNumber;
		} else {
			return a.price < b.price; 
		}
	}
};
struct sellComparator {
	bool operator() (orderStruct a, orderStruct b) {
		if(a.price == b.price) {
			return a.idNumber > b.idNumber; 
		} else {
			return a.price > b.price;
		}
	}
};
struct stringComparator {
	bool operator() (string a, string b) {
		return a > b; 
	}
};

typedef unordered_map<string, 
		priority_queue<orderStruct, 
		vector<orderStruct>, 
		buyComparator>> buyMap;
typedef unordered_map<string, 
		priority_queue<orderStruct, 
		vector<orderStruct>, 
		sellComparator>> sellMap;
typedef priority_queue<orderStruct, 
		vector<orderStruct>, 
		sellComparator> sellQueue; 
typedef priority_queue<orderStruct, 
		vector<orderStruct>, 
		buyComparator> buyQueue;
typedef	unordered_map<string, 
		medianDataStructure> medianMap;
typedef	unordered_map<string, 
		clientStocks> clientMap; 
typedef	priority_queue<string, 
		vector<string>, 
		stringComparator> stringQueue; 
typedef	unordered_map<string, 
		bool> insiderMap;
typedef unordered_map<string,
		ttStruct> ttMap; 


void buySellCalcuations(buyMap &buyStructure, 
	sellMap &sellStructure, const long long int &timeStamp,
	const string &symbol,
	long long int &marketCommission,
	long long int &completedTrades,
	long long int &sharesTraded,
	long long int &moneyTransfered, 
	const bool &verbose, const bool &median, const bool &transfers,
	medianMap &medianSymbols, clientMap &clientTransfers,
	stringQueue &medianStrings, stringQueue &clientStrings,
	ostream &ssin, ttMap &timeTravelers, const bool &timeTravel);

void printVerbose(ostream &ssin, const orderStruct &buy, const orderStruct &sell,
	const long long int &shares, const string &symbol);

void printMedian(medianMap &medianSymbols, 
	priority_queue<string, vector<string>, stringComparator> &medianStrings,
	const orderStruct &buy, const orderStruct &sell, const string &symbol);

void calcMarketCommission(const orderStruct &buy, const orderStruct &sell,
	long long int &onePercent, long long int &buyPrice, long long int &sellPrice,
	const long long int &quantity); 

void calcTransfers(clientMap &clientTransfers, stringQueue &clientStrings, 
	const long long int quantity, const orderStruct &buy,
	const orderStruct &sell);

bool insiderCalculationB(const long long int &median, const sellMap &sellStructure,
	const string &symbol);

bool insiderCalculationS(const long long int &median, const buyMap &buyStructure,
	const string &symbol);

void calcTimeTravel(ttStruct &timeTraveler, 
	const orderStruct &order, 
	const long long int &timeStamp,
	const string action);

int main(int argc, char *argv[]) {
	std::ios_base::sync_with_stdio(false); 

	struct option longOpts[] = {
		{"summary", no_argument, NULL, 's'},
		{"verbose", no_argument, NULL, 'v'},
		{"median", no_argument, NULL, 'm'},
		{"transfers", no_argument, NULL, 't'},
		{"insider", required_argument, NULL, 'i'},
		{"ttt", required_argument, NULL, 'g'},
		{"help", no_argument, NULL, 'h'},
	};

	int opt = 0, index = 0; 
	bool summary = false, verbose = false, median = false, transfers = false, 
	insider = false, timeTravel = false; 
	insiderMap insiders; 
	vector<string> timeTravelers; 
	ttMap mapOfTT; 

	while((opt = getopt_long(argc, argv, "svmti:g:h", longOpts, &index)) != -1) {
		switch(opt) {
			case 's':
				summary = true;
				break;
			case 'v':
				verbose = true;
				break;
			case 'm':
				median = true;
				break;
			case 't':
				transfers = true;
				break;
			case 'i':
				insider = true;
				insiders.insert({optarg, true}); 
				break;
			case 'g':
				timeTravel = true; 
				timeTravelers.push_back(optarg); 
				ttStruct temp; 
				temp.timestamp1 = -1;
				temp.timestamp2 = -1;
				temp.highestSell = LLONG_MAX; 
				temp.lowestBuy = -1; 
				temp.ttStamp1 = -1;
				temp.ttStamp2 = -1; 
				mapOfTT.insert({optarg, temp}); 
				break;
			case 'h':
				cout << "This program will simulate a stock market.\n";
				cout << "There are 6 different flags that can be used:\n";
				cout << "--summary or -s: Prints a summary\n";
				cout << "--verbose or -v: Print more info during trades\n";
				cout << "--median or -m: Print median match price\n";
				cout << "--transfers or -t: net amount of funds by clients\n";
				cout << "--insider or -i: Gives program advantage over others";
				cout << "--ttt or -g: Shows what was best time to buy/sell\n";
				cout << "--help or -h: Prints out this help screen\n";
				cout.flush(); 
				exit(0); 
		}
	}
	//Declare Variables and Code begins down here after get_opt
	stringstream ssin; 
	long long int timestamp = 0;
	string price;
	string quantity;
	string clientName;
	string action;
	string symbol; 
	buyMap buyStructure;
	sellMap sellStructure; 
	medianMap medianSymbols; 	
	stringQueue medianStrings; 
	stringQueue clientStrings; 
	clientMap clientTransfers; 

	//Read in from the input file
	string inputFormat; 
	cin >> inputFormat; 

	long long int currentTimeStamp = 0; 
	long long int idNumber = 0; 
	long long int marketCommission = 0; 
	long long int sharesTraded = 0;
	long long int completedTrades = 0; 
	long long int moneyTransfered = 0;  

	if(inputFormat == "TL") {
		cin >> timestamp ; 
		while(!cin.eof()) {

			orderStruct order;
			order.idNumber = idNumber; 
			//Read in the time stamp 
			
			if(cin.fail() || timestamp < currentTimeStamp) {
					cout << timestamp << endl;
					cerr << "Not an int" << endl;
					exit(1); 
			} 
		
			if(timestamp != currentTimeStamp) {
				if(median) {
					stringQueue temp; 
					while(!medianStrings.empty()) {
						string eSymbol = medianStrings.top();
						medianStrings.pop(); 
						temp.push(eSymbol); 
						ssin << "Median match price of " << eSymbol;
						ssin << " at time " << currentTimeStamp << " is $";
						ssin << medianSymbols.at(eSymbol).getMedian() << "\n";
					}
					medianStrings = temp; 
				}
				currentTimeStamp = timestamp; 
			}

			//Read in the client name
			cin >> clientName;
			for(int i = 0; i < clientName.size(); i++) {
				if(!isalnum(clientName[i])) {
					if(clientName[i] != '_') {
						cerr << "Make sure it is alphanum or _ for names\n"; 
						exit(1);
					}
				}
			}
			order.clientName = clientName; 

			//Read in his action
			cin >> action;
			if(action != "BUY" && action != "SELL") {
				cerr << "Not buy or sell" << endl;
				exit(1); 
			}

			//Read in the symbol
			cin >> symbol;
			if(symbol.size() > 5) {
				cerr << "Size of symbol larger than 5" << endl;
				exit(1); 
			}
			for(int i = 0; i < symbol.size(); i++) {
				if(!isalnum(symbol[i])) {
					if(symbol[i] != '_' && symbol[i] != '.') {
						cerr << "Non alphanumeric equity symbol or _" << endl;
						exit(1);
					}
				}
			}

			//Read in the price
			cin >> price;
			if(price.at(0) != '$') {
				cerr << "The dollar sign wasn't there" << endl;
				exit(1);
			} else {
				price.erase(0,1); 
				long long int priceInt = atoi(price.c_str()); 
				if(priceInt < 1) {
					cerr << "Less than 1" << endl;
					exit(1); 
				} else {
					order.price = priceInt; 
				}
			}

			//Read in the quantity 
			cin  >> quantity; 
			if(quantity.at(0) != '#') {
				cerr << "Make sure there is a #" << endl;
				exit(1);
			} else {
				quantity.erase(0,1); 
				long long int quantityInt = atoi(quantity.c_str()); 
				order.quantity = quantityInt; 
			}

			//Place things into the client structure now. 
			if(action == "BUY") {
				//First check if the key is in the map
				if(buyStructure.find(symbol) == buyStructure.end()) {
					buyQueue newQueue; 
					newQueue.push(order);
					buyStructure.insert({symbol, newQueue});
				} else {
					buyStructure.at(symbol).push(order); 
				}
			} else if (action == "SELL") {
				//Check if this key is in the map first
				if(sellStructure.find(symbol) == sellStructure.end()) {
					sellQueue newQueue2; 
					newQueue2.push(order);
					sellStructure.insert({symbol, newQueue2}); 
				} else {
					sellStructure.at(symbol).push(order);
				}
			}
			if(timeTravel) {
				if(mapOfTT.find(symbol) != mapOfTT.end()) {
					calcTimeTravel(mapOfTT.at(symbol), order, timestamp, action); 
				}
			}

			if(transfers) {
				if(clientTransfers.find(clientName) == clientTransfers.end()) {
					clientStrings.push(clientName); 
					clientStocks buyer;
					buyer.numberSold = 0; 
					buyer.numberBought = 0; 
					buyer.netProfit = 0; 
					clientTransfers.insert({clientName, buyer});
				}
			}
			buySellCalcuations(buyStructure, 
				sellStructure, timestamp,
				symbol, 
				marketCommission,
				completedTrades,
				sharesTraded,
				moneyTransfered,
				verbose, median, transfers,
				medianSymbols, clientTransfers,
				medianStrings, clientStrings,
				ssin, mapOfTT, timeTravel); 
			
			
			idNumber++; 
			if(insider) {
				if(insiders.find(symbol) != insiders.end()) {
					long long int medianValue; 
					orderStruct insider; 
					insider.clientName = "INSIDER_" + symbol;

					if(transfers) {
						if(clientTransfers.find(insider.clientName) == clientTransfers.end()) {
							clientStrings.push(insider.clientName); 
							clientStocks buyer;
							buyer.numberSold = 0; 
							buyer.numberBought = 0; 
							buyer.netProfit = 0; 
							clientTransfers.insert({insider.clientName, buyer});
						}
					}

					if(medianSymbols.find(symbol) == medianSymbols.end()) {
						cin >> timestamp; 
						continue;
					} else {
						medianValue = medianSymbols.at(symbol).getMedian(); 
					}
					if(sellStructure.find(symbol) != sellStructure.end()) {
						if(insiderCalculationB(medianValue, sellStructure, symbol)) {
							insider.idNumber = idNumber;
							insider.price = sellStructure.at(symbol).top().price;
							insider.quantity = sellStructure.at(symbol).top().quantity; 
							if(buyStructure.find(symbol) != buyStructure.end()) {
								buyStructure.at(symbol).push(insider); 
							} else {
								buyQueue newQueue;
								newQueue.push(insider); 
								buyStructure.insert({symbol, newQueue}); 
							}
							if(timeTravel) {
								if(mapOfTT.find(symbol) != mapOfTT.end()) {
									calcTimeTravel(mapOfTT.at(symbol), insider, timestamp, "BUY"); 
								}
							}

							buySellCalcuations(buyStructure, 
							sellStructure, timestamp,
							symbol, 
							marketCommission,
							completedTrades,
							sharesTraded,
							moneyTransfered, 
							verbose, median, transfers,
							medianSymbols, clientTransfers,
							medianStrings, clientStrings,
							ssin, mapOfTT, timeTravel); 
							
							idNumber++; 
						} 
					}
					
					if(buyStructure.find(symbol) != buyStructure.end()) {
						if(insiderCalculationS(medianValue, buyStructure, symbol)) {
							insider.idNumber = idNumber;
							insider.price = buyStructure.at(symbol).top().price;
							insider.quantity = buyStructure.at(symbol).top().quantity;
							if(sellStructure.find(symbol) != sellStructure.end()) {
								sellStructure.at(symbol).push(insider); 
							} else {
								sellQueue newQueue;
								newQueue.push(insider); 
								sellStructure.insert({symbol, newQueue}); 
							}
							
							if(timeTravel) {
								if(mapOfTT.find(symbol) != mapOfTT.end()) {
									calcTimeTravel(mapOfTT.at(symbol), insider, timestamp, "SELL"); 
								}
							}

							buySellCalcuations(buyStructure, 
							sellStructure, timestamp, 
							symbol, 
							marketCommission,
							completedTrades,
							sharesTraded,
							moneyTransfered, 
							verbose, median, transfers,
							medianSymbols, clientTransfers,
							medianStrings, clientStrings,
							ssin, mapOfTT, timeTravel); 

							idNumber++; 
						}
					}
				}
			} 
			cin >> timestamp; 
		}
	} else if (inputFormat == "PR") {
		string trash;
		
		long long int seed, numberOfOrders;
		char lastClient, lastEquity;
		double arrivalRate; 
		long long int priceInt; 
		long long int quantityInt; 
		cin >> trash; 
		cin >> seed >> trash >> numberOfOrders >> trash >> lastClient;
		cin >> trash >> lastEquity >> trash >> arrivalRate; 
	
		std::mt19937 gen(seed);
		std::uniform_int_distribution<char> clients('a', lastClient);
		std::uniform_int_distribution<char> equities('A', lastEquity); 
		std::exponential_distribution<> arrivals(arrivalRate);
		std::bernoulli_distribution buy_or_sell;
		std::uniform_int_distribution<> price(2,11);
		std::uniform_int_distribution<> quantity(1, 30); 

		long long int GENERATOR_TIMESTAMP = 0;
		timestamp = GENERATOR_TIMESTAMP; 
		
		long long int counter = 0; 
		while(counter < numberOfOrders) {
			orderStruct order; 

			if(timestamp != GENERATOR_TIMESTAMP) {
				if(median) {
					stringQueue temp; 
					while(!medianStrings.empty()) {
						string eSymbol = medianStrings.top();
						medianStrings.pop(); 
						temp.push(eSymbol); 
						ssin << "Median match price of " << eSymbol;
						ssin << " at time " << timestamp << " is $";
						ssin << medianSymbols.at(eSymbol).getMedian() << "\n";
					}
					medianStrings = temp; 
				}
				timestamp = GENERATOR_TIMESTAMP; 
			} 
			
			GENERATOR_TIMESTAMP = GENERATOR_TIMESTAMP + floor(arrivals(gen) + 0.5); 

			clientName = "C_";
			clientName += clients(gen);
			action = (buy_or_sell(gen) ? "BUY" : "SELL"); 
			symbol = "E_";
			symbol += equities(gen);
			priceInt = 5 * price(gen); 
			quantityInt = quantity(gen);

			order.idNumber = idNumber; 
			order.price = priceInt;
			order.quantity = quantityInt;
			order.clientName = clientName;
			//Place things into the client structure now. 
			if(action == "BUY") {
				//First check if the key is in the map
				if(buyStructure.find(symbol) == buyStructure.end()) {
					buyQueue newQueue; 
					newQueue.push(order);
					buyStructure.insert({symbol, newQueue});
				} else {
					buyStructure.at(symbol).push(order); 
				}
			} else if (action == "SELL") {
				//Check if this key is in the map first
				if(sellStructure.find(symbol) == sellStructure.end()) {
					sellQueue newQueue2; 
					newQueue2.push(order);
					sellStructure.insert({symbol, newQueue2}); 
				} else {
					sellStructure.at(symbol).push(order);
				}
			}
			if(timeTravel) {
				if(mapOfTT.find(symbol) != mapOfTT.end()) {
					calcTimeTravel(mapOfTT.at(symbol), order, timestamp, action); 
				}
			}

			if(transfers) {
				if(clientTransfers.find(clientName) == clientTransfers.end()) {
					clientStrings.push(clientName); 
					clientStocks buyer;
					buyer.numberSold = 0; 
					buyer.numberBought = 0; 
					buyer.netProfit = 0; 
					clientTransfers.insert({clientName, buyer});
				}
			}

			buySellCalcuations(buyStructure, 
				sellStructure, timestamp,
				symbol, 
				marketCommission,
				completedTrades,
				sharesTraded,
				moneyTransfered,
				verbose, median, transfers,
				medianSymbols, clientTransfers,
				medianStrings, clientStrings,
				ssin, mapOfTT, timeTravel); 
			
			counter++;
			idNumber++;
			if(insider) {
				if(insiders.find(symbol) != insiders.end()) {
					long long int medianValue; 
					orderStruct insider; 
					insider.clientName = "INSIDER_" + symbol; 

					if(transfers) {
						if(clientTransfers.find(insider.clientName) == clientTransfers.end()) {
							clientStrings.push(insider.clientName); 
							clientStocks buyer;
							buyer.numberSold = 0; 
							buyer.numberBought = 0; 
							buyer.netProfit = 0; 
							clientTransfers.insert({insider.clientName, buyer});
						}
					}

					if(medianSymbols.find(symbol) == medianSymbols.end()) {
						continue;
					} else {
						medianValue = medianSymbols.at(symbol).getMedian(); 
					}

					if(sellStructure.find(symbol) != sellStructure.end()) {
						if(insiderCalculationB(medianValue, sellStructure, symbol)) {
							insider.idNumber = idNumber;
							insider.price = sellStructure.at(symbol).top().price;
							insider.quantity = sellStructure.at(symbol).top().quantity; 
							if(buyStructure.find(symbol) != buyStructure.end()) {
								buyStructure.at(symbol).push(insider); 
							} else {
								buyQueue newQueue;
								newQueue.push(insider); 
								buyStructure.insert({symbol, newQueue}); 
							}
							
							if(timeTravel) {
								if(mapOfTT.find(symbol) != mapOfTT.end()) {
									calcTimeTravel(mapOfTT.at(symbol), insider, timestamp, "BUY"); 
								}
							}
							
							buySellCalcuations(buyStructure, 
							sellStructure, timestamp,
							symbol, 
							marketCommission,
							completedTrades,
							sharesTraded,
							moneyTransfered, 
							verbose, median, transfers,
							medianSymbols, clientTransfers,
							medianStrings, clientStrings,
							ssin, mapOfTT, timeTravel); 	
							idNumber++; 
						} 
					}
					
					if(buyStructure.find(symbol) != buyStructure.end()) {
						if(insiderCalculationS(medianValue, buyStructure, symbol)) {
							insider.idNumber = idNumber;
							insider.price = buyStructure.at(symbol).top().price;
							insider.quantity = buyStructure.at(symbol).top().quantity;
							if(sellStructure.find(symbol) != sellStructure.end()) {
								sellStructure.at(symbol).push(insider); 
							} else {
								sellQueue newQueue;
								newQueue.push(insider); 
								sellStructure.insert({symbol, newQueue}); 
							}
							
							if(timeTravel) {
								if(mapOfTT.find(symbol) != mapOfTT.end()) {
									calcTimeTravel(mapOfTT.at(symbol), insider, timestamp, "SELL"); 
								}
							}

							buySellCalcuations(buyStructure, 
							sellStructure, timestamp, 
							symbol, 
							marketCommission,
							completedTrades,
							sharesTraded,
							moneyTransfered, 
							verbose, median, transfers,
							medianSymbols, clientTransfers,
							medianStrings, clientStrings,
							ssin, mapOfTT, timeTravel); 
							idNumber++; 
						}
					}
				}
			}
		}
	} else {
		cerr <<"Make sure you entered a correct input format\n";
		cout.flush();
		exit(1); 
	}
	if(median) {
		while(!medianStrings.empty()) {
			string eSymbol = medianStrings.top();
			medianStrings.pop(); 
			ssin << "Median match price of " << eSymbol;
			ssin << " at time " << timestamp << " is $";
			ssin << medianSymbols.at(eSymbol).getMedian() << "\n";
		}
	}
	ssin << "---End of Day---\n"; 

	if(summary) {
		ssin << "Commission Earnings: $" << marketCommission << "\n";
		ssin << "Total Amount of Money Transferred: $" << moneyTransfered << "\n";
		ssin << "Number of Completed Trades: " << completedTrades << "\n";
		ssin << "Number of Shares Traded: " << sharesTraded << "\n"; 
	}

	if(transfers) {
		while(!clientStrings.empty()) {
			string temp = clientStrings.top();
			clientStrings.pop();
			clientStocks person;
			if(clientTransfers.find(temp) == clientTransfers.end()) {
				person.numberBought = 0;
				person.numberSold = 0;
				person.netProfit = 0;
			} else {
			 	person = clientTransfers.at(temp); 
			}
			ssin << temp << " bought " << person.numberBought << " and sold ";
			ssin << person.numberSold << " for a net transfer of $";
			ssin << person.netProfit << "\n"; 
		}
	}

	if(timeTravel) {
		for(int i = 0; i < timeTravelers.size(); i++) {
			ttStruct temp = mapOfTT.at(timeTravelers.at(i)); 
			ssin << "Time travelers would buy " << timeTravelers.at(i);
			ssin << " at time: " << temp.ttStamp1 << " and sell it at time: ";
			ssin << temp.ttStamp2 << "\n";
		}
	}
	cout << ssin.str();
	cout.flush(); 

	return 0;
}

//This function does all the buy/selling logic in the market
void buySellCalcuations(buyMap &buyStructure, 
	sellMap &sellStructure, const long long int &timeStamp,
	const string &symbol,
	long long int &marketCommission,
	long long int &completedTrades,
	long long int &sharesTraded,
	long long int &moneyTransfered, 
	const bool &verbose, const bool &median, const bool &transfers,
	medianMap &medianSymbols, clientMap &clientTransfers,
	stringQueue &medianStrings,
	stringQueue &clientStrings,
	ostream &ssin, ttMap &timeTravelers, const bool &timeTravel) {

	if(buyStructure.find(symbol) == buyStructure.end()) {
		return; 
	} 

	if(sellStructure.find(symbol) == sellStructure.end()) {
		return; 
	} 

	while(!buyStructure.at(symbol).empty() 
		&& !sellStructure.at(symbol).empty()) {

		orderStruct sell = sellStructure.at(symbol).top();
		orderStruct buy = buyStructure.at(symbol).top();
		// ssin << buy.clientName << " " << buy.price << " is the buyer. Seller is ";
		// ssin << sell.clientName << " " << sell.price << endl;
		sellStructure.at(symbol).pop();
		buyStructure.at(symbol).pop(); 
		
		if(sell.price > buy.price) {
			sellStructure.at(symbol).push(sell);
			buyStructure.at(symbol).push(buy); 
			break; 
		} else {
			completedTrades++; 
			long long int buyQuantity = buy.quantity;
			long long int sellQuantity = sell.quantity;
			if(buyQuantity >= sellQuantity) {

				if(transfers) {
					calcTransfers(clientTransfers, clientStrings, sellQuantity, buy, sell);
				}

				long long int onePercent = 0;
				long long int buyPrice = 0;
				long long int sellPrice = 0; 

				calcMarketCommission(buy, sell, onePercent, buyPrice, sellPrice, sellQuantity); 

				marketCommission = marketCommission + onePercent * 2; 
				buyQuantity = buyQuantity - sellQuantity; 

				if(verbose) {
					printVerbose(ssin, buy, sell, sellQuantity, symbol); 
				}

				printMedian(medianSymbols, medianStrings, buy, sell, symbol); 
				

				sharesTraded = sharesTraded + sellQuantity; 
				if(buy.idNumber < sell.idNumber) {
					moneyTransfered = moneyTransfered + (buy.price * sellQuantity);
				} else {
					moneyTransfered = moneyTransfered + (sell.price * sellQuantity); 
				}
				
				sellQuantity = 0;  
				
				if(buyQuantity != 0) {

					buy.quantity = buyQuantity; 
					//ssin << "Buyer: " << buy.clientName << " " << buy.price << " " << buy.quantity << endl;
					buyStructure.at(symbol).push(buy); 
					orderStruct buy2= buyStructure.at(symbol).top();
					//ssin << "Buyer After: " << buy.clientName << " " << buy.price << " " << buy.quantity << endl;
				}	
				if(buyStructure.at(symbol).size() == 0) {
					buyStructure.erase(symbol); 
				}
				if(sellStructure.at(symbol).size() == 0) {
					sellStructure.erase(symbol); 
				}
			} else {

				if(transfers) {
					calcTransfers(clientTransfers, clientStrings, buyQuantity, buy, sell);
				}

				long long int onePercent = 0;
				long long int buyPrice = 0;
				long long int sellPrice = 0; 

				calcMarketCommission(buy, sell, onePercent, buyPrice, sellPrice, buyQuantity); 

				marketCommission = marketCommission + onePercent * 2; 
				sellQuantity = sellQuantity - buyQuantity;

				if(verbose) {
					printVerbose(ssin, buy, sell, buyQuantity, symbol); 
				}

				printMedian(medianSymbols, medianStrings, buy, sell, symbol); 


				sharesTraded = sharesTraded + buyQuantity; 
				if(buy.idNumber < sell.idNumber) {
					moneyTransfered = moneyTransfered + (buy.price * buyQuantity);
				} else {
					moneyTransfered = moneyTransfered + (sell.price * buyQuantity); 
				}
				buyQuantity = 0; 

				if(sellQuantity != 0) {
					sell.quantity = sellQuantity; 
					sellStructure.at(symbol).push(sell); 
				}
				if(buyStructure.at(symbol).size() == 0) {
					buyStructure.erase(symbol); 
				}
				if(sellStructure.at(symbol).size() == 0) {
					sellStructure.erase(symbol); 
				}
			}
			if(sellStructure.find(symbol) == sellStructure.end() or
				buyStructure.find(symbol) == buyStructure.end()) {
				break;
			}
		}
	}
	return; 
}

void printVerbose(ostream &ssin, const orderStruct &buy, const orderStruct &sell,
	const long long int &shares, const string &symbol) {
	ssin << buy.clientName << " purchased " << shares << " shares";
	ssin << " of " << symbol << " from " << sell.clientName << " for $";
	if(buy.idNumber < sell.idNumber) {
		ssin << buy.price << "/share\n"; 
	} else {
		ssin << sell.price << "/share\n"; 
	}
}

void printMedian(medianMap &medianSymbols, 
	priority_queue<string, vector<string>, stringComparator> &medianStrings,
	const orderStruct &buy, const orderStruct &sell, const string &symbol) {

	if(medianSymbols.find(symbol) == medianSymbols.end()) {
		medianDataStructure mData;
		medianStrings.push(symbol);
		if(buy.idNumber < sell.idNumber) {
			mData.insertElement(buy.price);
		} 
		else {
			mData.insertElement(sell.price); 
		}
		medianSymbols.insert({symbol, mData}); 
	} else {
		if(buy.idNumber < sell.idNumber) {
			medianSymbols.at(symbol).insertElement(buy.price);
		} 
		else {
			medianSymbols.at(symbol).insertElement(sell.price); 
		}
	}
}

void calcMarketCommission(const orderStruct &buy, const orderStruct &sell,
	long long int &onePercent, long long int &buyPrice, long long int &sellPrice,
	const long long int &quantity) {
		if(buy.idNumber < sell.idNumber) {
			onePercent = (quantity * buy.price)/100; 
			buyPrice = (quantity * buy.price) + onePercent;
			sellPrice = (quantity * buy.price) - onePercent; 
		} else {
			onePercent = (quantity * sell.price)/100; 
			buyPrice = (quantity * sell.price) + onePercent;
			sellPrice = (quantity * sell.price) - onePercent; 
		}
}

void calcTransfers(clientMap &clientTransfers, stringQueue &clientStrings, 
	const long long int quantity, const orderStruct &buy,
	const orderStruct &sell) {
	long long int totalTransfered = 0; 

	//This is the initial check for the buyer
	clientTransfers.at(buy.clientName).numberBought
		= clientTransfers.at(buy.clientName).numberBought + quantity; 
	if(buy.idNumber < sell.idNumber) {
		totalTransfered = buy.price * quantity;
	} else {
		totalTransfered = sell.price * quantity; 
	}
	clientTransfers.at(buy.clientName).netProfit 
		= clientTransfers.at(buy.clientName).netProfit - totalTransfered;

	//Check for the seller
	clientTransfers.at(sell.clientName).numberSold 
		= clientTransfers.at(sell.clientName).numberSold + quantity; 
	if(buy.idNumber < sell.idNumber) {
		totalTransfered = buy.price * quantity;
	} else {
		totalTransfered = sell.price * quantity; 
	}
	clientTransfers.at(sell.clientName).netProfit
		= clientTransfers.at(sell.clientName).netProfit + totalTransfered;
}

bool insiderCalculationB(const long long int &median, const sellMap &sellStructure,
	const string &symbol) {
	long long int minProfit = median / 10; 
	orderStruct minOrder = sellStructure.at(symbol).top();

	long long int profit = median - minOrder.price; 
	
	if(profit > minProfit) {
		return true;
	}
	return false; 
}

bool insiderCalculationS(const long long int &median, const buyMap &buyStructure,
	const string &symbol) {
	long long int minProfit = median / 10; 
	orderStruct maxOrder = buyStructure.at(symbol).top();
	long long int profit = maxOrder.price - median; 
	if(profit > minProfit) {
		return true;
	}
	return false; 
}

void calcTimeTravel(ttStruct &timeTraveler, 
	const orderStruct &order, 
	const long long int &timeStamp,
	const string action) {

	if(timeTraveler.lowestBuy == -1) {
		if(action == "SELL") {
			timeTraveler.lowestBuy = order.price;
			timeTraveler.timestamp1 = timeStamp; 
			timeTraveler.order1 = order.idNumber;
		} else {
			return;
		}
	} 
	else if(timeTraveler.highestSell == LLONG_MAX) {
		if(action == "BUY") {
			timeTraveler.highestSell = order.price;
			timeTraveler.timestamp2 = timeStamp;
			timeTraveler.order2 = order.idNumber;
		} else {
			if(order.price < timeTraveler.lowestBuy) {
				timeTraveler.lowestBuy = order.price;
				timeTraveler.timestamp1 = timeStamp;
				timeTraveler.order1 = order.idNumber;
			}
		}
	} 
	else {
		if(action == "SELL") {
			if(order.price < timeTraveler.lowestBuy) {
				timeTraveler.lowestBuy = order.price;
				timeTraveler.timestamp1 = timeStamp;
				timeTraveler.order1 = order.idNumber;
			}
		} else if (action == "BUY") {
			if((order.price - timeTraveler.lowestBuy) > timeTraveler.maxProfit) {
				timeTraveler.highestSell = order.price;
				timeTraveler.timestamp2 = timeStamp;
				timeTraveler.order2 = order.idNumber; 
			}
		}
	}

	if(timeTraveler.order1 < timeTraveler.order2) {
		timeTraveler.maxProfit = timeTraveler.highestSell - timeTraveler.lowestBuy; 
		timeTraveler.ttStamp1 = timeTraveler.timestamp1;
		timeTraveler.ttStamp2 = timeTraveler.timestamp2; 
	}
}
