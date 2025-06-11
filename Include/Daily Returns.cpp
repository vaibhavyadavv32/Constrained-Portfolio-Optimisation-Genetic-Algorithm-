#include "Daily Returns.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>        // For log()
#include <filesystem>   // For directory creation

using namespace std;

// Converts string to double safely with trimming and fallback
double safeStod(const string& s) 
{
    try {
        string trimmed = s;
        trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r\f\v"));
        trimmed.erase(trimmed.find_last_not_of(" \t\n\r\f\v") + 1);

        if (trimmed.empty()) return 0.0;

        size_t pos;
        double val = stod(trimmed, &pos);
        if (pos != trimmed.length()) 
        {
            cerr << "Partial conversion: '" << s << "' -> " << val << endl;
            return 0.0;
        }
        return val;
    } catch (const exception& e) 
    {
        cerr << "Conversion error for '" << s << "': " << e.what() << endl;
        return 0.0;
    }
}

bool DataReader::processAndSaveDailyReturns(const string& input_filepath, const string& output_filepath) 
{
    ifstream fin(input_filepath);
    if (!fin.is_open()) 
    {
        cerr << "Failed to open input file: " << input_filepath << endl;
        return false;
    }

    filesystem::path output_path = filesystem::path(output_filepath).parent_path();
    if (!output_path.empty() && !filesystem::exists(output_path)) 
    {
        try 
        {
            filesystem::create_directories(output_path);
            cout << "Created output directory: " << output_path << endl;
        } 
        catch (const filesystem::filesystem_error& e) 
        {
            cerr << "Directory creation failed: " << e.what() << endl;
            return false;
        }
    }

    ofstream fout(output_filepath);
    if (!fout.is_open()) 
    {
        cerr << "Failed to open output file: " << output_filepath << endl;
        return false;
    }

    string line;
    getline(fin, line); // Header
    stringstream ss(line);
    string token;
    getline(ss, token, ','); // Skip "Date"

    assetTickers.clear();
    while (getline(ss, token, ',')) 
    {
        assetTickers.push_back(token);
    }

    fout << "Date";
    for (const string& ticker : assetTickers) 
    {
        fout << "," << ticker;
    }
    fout << endl;

    vector<double> prev_prices;
    bool first = true;

    while (getline(fin, line)) 
    {
        stringstream ss_line(line);
        string date;
        getline(ss_line, date, ',');

        vector<double> curr_prices, curr_returns;
        string price_str;

        while (getline(ss_line, price_str, ',')) 
        {
            curr_prices.push_back(safeStod(price_str));
        }

        prices.push_back(curr_prices);
        fout << date;

        if (first) 
        {
            curr_returns.assign(curr_prices.size(), 0.0);
            for (double ret : curr_returns) fout << "," << ret;
            first = false;
        } 
        else 
        {
            for (size_t i = 0; i < curr_prices.size(); ++i) 
            {
                double log_ret = 0.0;
                if (prev_prices[i] > 0.0 && curr_prices[i] > 0.0)
                    log_ret = log(curr_prices[i] / prev_prices[i]);
                curr_returns.push_back(log_ret);
                fout << "," << log_ret;
            }
        }

        dailyReturns.push_back(curr_returns);
        fout << endl;
        prev_prices = curr_prices;
    }

    fin.close();
    fout.close();

    cout << "Saved daily log returns to: " << output_filepath << endl;
    return true;
}