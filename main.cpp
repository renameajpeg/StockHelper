#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <map>
#include <limits>
using namespace std;


struct Stock {
    string ticker;
    string industry;
    double highPrice;
    double lowPrice;
    double percentChange;
    double riskLevel; // number to represent risk (low = 1, medium = 2, high = 3)

    Stock(string t, string i, double high, double low, double risk)
        : ticker(t), industry(i), highPrice(high), lowPrice(low), riskLevel(risk) {
        percentChange = ((high - low) / low) * 100;
    }
};

// function for max-heap
class CompareStock {
public:
    bool operator()(const Stock& s1, const Stock& s2) {
        return s1.percentChange < s2.percentChange; // max heap based on percent change
    }
};

class Heap {
    vector<Stock> data;

    void heapifyUp(int index) {
        while (index > 0 && data[parent(index)].percentChange < data[index].percentChange) {
            swap(data[parent(index)], data[index]);
            index = parent(index);
        }
    }

    void heapifyDown(int index) {
        int leftChild = left(index);
        int rightChild = right(index);
        int largest = index;

        if (leftChild < data.size() && data[leftChild].percentChange > data[largest].percentChange) {
            largest = leftChild;
        }
        if (rightChild < data.size() && data[rightChild].percentChange > data[largest].percentChange) {
            largest = rightChild;
        }
        if (largest != index) {
            swap(data[index], data[largest]);
            heapifyDown(largest);
        }
    }

    int parent(int index) { return (index - 1) / 2; }
    int left(int index) { return 2 * index + 1; }
    int right(int index) { return 2 * index + 2; }

public:
    void insert(const Stock& stock) {
        data.push_back(stock);
        heapifyUp(data.size() - 1);
    }

    Stock extractMax() {
        if (data.empty()) throw runtime_error("Heap is empty");

        Stock maxStock = data[0];
        data[0] = data.back();
        data.pop_back();
        heapifyDown(0);

        return maxStock;
    }

    bool isEmpty() const {
        return data.empty();
    }
};

// class for map and associated functions
class MapSort {
    map<string, Stock> stocks;
public:
    void insert(Stock stock) {
        stocks.insert({ stock.ticker, stock });
    }

    Stock getMax() {
        Stock output = stocks.begin()->second;
        int tempPct = output.percentChange;
        // returns from final sorted map, prioritizes lowest highPrice value
        for (auto it : stocks) {
            if (it.second.percentChange >= tempPct) {
                output = it.second;
                tempPct = output.percentChange;
            }
        }
        return output;
    }

    void mapIterate(int len) {
        int i = 2;
        for (auto iter : stocks) {
            if (i >= len) {
                break;
            }
            Stock iterated = iter.second;
            cout << i << ". " << iterated.ticker << " (" << iterated.industry << "): " << iterated.percentChange
                << "% change, High Price: $" << iterated.highPrice << ", Low Price: $" << iterated.lowPrice << endl;
            i++;
        }
    }

    bool isEmpty() {
        return stocks.empty();
    }
};


// function to read CSV and load data
vector<Stock> loadStocks(const string& filename) {
    vector<Stock> stocks;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return stocks;
    }

    string line, ticker, industry;
    string high, low, risk;
    double highDouble, lowDouble, riskDouble;


    long count = 0;


    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, ticker, '\t');
        getline(ss, industry, '\t');
        getline(ss, high, '\t');
        getline(ss, low, '\t');
        getline(ss, risk, '\n');
        highDouble = stod(high);
        lowDouble = stod(low);
        riskDouble = stod(risk);
        stocks.emplace_back(ticker, industry, highDouble, lowDouble, riskDouble);
    }

    file.close();
    return stocks;
}

// function to prompt user and filter
vector<Stock> filterStocks(const vector<Stock>& stocks, double budget, int riskTolerance, const string& preferredIndustry) {
    vector<Stock> filtered;
    cout << "Debugging Filter Logic:\n";

    for (const auto& stock : stocks) {
        if (stock.highPrice <= budget && stock.riskLevel <= riskTolerance &&
            (preferredIndustry.empty() || stock.industry == preferredIndustry)) {
            filtered.push_back(stock);
        }
    }

    cout << "Total matching stocks: " << filtered.size() << endl;
    return filtered;
}


int main() {
    vector<Stock> stocks = loadStocks("fake_stock_data.txt");
    //cout << "Stock: " << stocks.begin()->ticker << " " << stocks.begin()->industry << endl;

    // user inputs
    double budget;
    int riskTolerance;
    string preferredIndustry;

    bool switcher = true;
    while (switcher) {
        cout << "Enter your budget: ";
        cin >> budget;
        cout << "Enter your risk tolerance (1 = Low, 2 = Medium, 3 = High): ";
        cin >> riskTolerance;
        cin.ignore();
        cout << "Enter your preferred industry (or leave blank for no preference): ";
        getline(cin, preferredIndustry);


        // filter stocks based on input
        vector<Stock> filteredStocks = filterStocks(stocks, budget, riskTolerance, preferredIndustry);

        // prompt user for choice of data structure
        int structChoice;
        cout << "Enter your preferred data structure ('1' for Heap, '2' for Map): " << endl;
        cin >> structChoice;

        //  uses heap for recommendations
        if (structChoice == 1) {
            Heap stockHeap;
            for (const auto& stock : filteredStocks) {
                stockHeap.insert(stock);
            }
            if (stockHeap.isEmpty()) {
                cout << "No stocks match your criteria." << endl;
            }
            else {
                cout << "Top recommended stock:" << endl;
                Stock topStock = stockHeap.extractMax();
                cout << topStock.ticker << " (" << topStock.industry << "): " << topStock.percentChange
                    << "% change, High Price: $" << topStock.highPrice << ", Low Price: $" << topStock.lowPrice << endl;
            }
            /*for (int i = 2; i < 11; i++) {
                cout << i << ". ";
                Stock topStock = stockHeap.extractMax();
                cout << topStock.ticker << " (" << topStock.industry << "): " << topStock.percentChange
                    << "% change, High Price: $" << topStock.highPrice << ", Low Price: $" << topStock.lowPrice << endl;
            }*/
        }
        // uses map for recommendations
        else if (structChoice == 2) {
            MapSort stockMap;
            for (auto stock : filteredStocks) {
                stockMap.insert(stock);
            }
            if (stockMap.isEmpty()) {
                cout << "No stocks match your criteria." << endl;
            }
            else {
                cout << "Top recommended stock:" << endl;
                Stock topStock = stockMap.getMax();
                cout << topStock.ticker << " (" << topStock.industry << "): " << topStock.percentChange
                    << "% change, High Price: $" << topStock.highPrice << ", Low Price: $" << topStock.lowPrice << endl;
            }
            //**make so highest % change values are in top 10
            /*stockMap.mapIterate(11);*/
        }
        // error if input is invalid
        else {
            cout << "Invalid choice" << endl;
        }
        // continues or ends program loop 
        cout << "Enter 1 to continue or 0 to quit: " << endl;
        int userContinue;
        cin >> userContinue;
        if (userContinue == 0) {
            switcher = false;
        }
    }

    return 0;
}
