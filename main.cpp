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
        //cout << "high " <<  high << " low " << low << " risk " << risk << endl;
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
        /*cout << "Checking stock: " << stock.ticker << ", Industry: " << stock.industry
             << ", High Price: " << stock.highPrice << ", Risk: " << stock.riskLevel << endl;
        */
        if (stock.highPrice <= budget && stock.riskLevel <= riskTolerance &&
            (preferredIndustry.empty() || stock.industry == preferredIndustry)) {
            //cout << "Stock matches criteria: " << stock.ticker << endl;
            filtered.push_back(stock);
        }
    }

    cout << "Total matching stocks: " << filtered.size() << endl;
    return filtered;
}


int main() {
    vector<Stock> allStocks;

    //vector<string> files = {"fake_stock_data.txt"};
    //vector<string> files = {"AAPL.csv", "AMD.csv", "AMZN.csv", "CSCO.csv", "META.csv",
                            //"MSFT.csv", "NFLX.csv", "QCOM.csv", "SBUX.csv", "TSLA.csv", "overall_stock_data.csv"};
   /*for (const auto& file : files) {
        vector<Stock> stocks = loadStocks(file);
        allStocks.insert(allStocks.end(), stocks.begin(), stocks.end());
    }*/
    vector<Stock> stocks = loadStocks("fake_stock_data.txt");
    cout<<"Stock: "<<stocks.begin()->ticker<<" "<<stocks.begin()->industry<<endl;

    // user inputs
    double budget;
    int riskTolerance;
    string preferredIndustry;

    cout << "Enter your budget: ";
    cin >> budget;
    cout << "Enter your risk tolerance (1 = Low, 2 = Medium, 3 = High): ";
    cin >> riskTolerance;
    cin.ignore();
    cout << "Enter your preferred industry (or leave blank for no preference): ";
    getline(cin, preferredIndustry);
    cout<<budget<<" "<<riskTolerance<<" "<<preferredIndustry<<endl;


    // filter stocks based on input
    vector<Stock> filteredStocks = filterStocks(stocks, budget, riskTolerance, preferredIndustry);

    //  heap for recommendations
    Heap stockHeap;
    for (const auto& stock : filteredStocks) {
        stockHeap.insert(stock);
    }

    if (stockHeap.isEmpty()) {
        cout << "No stocks match your criteria." << endl;
    } else {
        cout << "Top recommended stock:" << endl;
        Stock topStock = stockHeap.extractMax();
        cout << topStock.ticker << " (" << topStock.industry << "): " << topStock.percentChange
             << "% change, High Price: $" << topStock.highPrice << ", Low Price: $" << topStock.lowPrice << endl;
    }

    return 0;
}
