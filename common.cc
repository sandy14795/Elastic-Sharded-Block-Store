#include<iostream>
#include<string>
#include<stdio.h>

using namespace std;

bool getArg(int argc, char** argv, string arg_str, string *ret_string, int pos) {
    string target_str;
    if (argc > 1) {
        string arg_val = argv[pos];
        size_t start_pos = arg_val.find(arg_str);
        if (start_pos != string::npos) {
            start_pos += arg_str.size();
            if (arg_val[start_pos] == '=') {
                target_str = arg_val.substr(start_pos + 1);
                ret_string->resize(target_str.size());
                ret_string->replace(0, target_str.size(), target_str);
                return true;
            } else {
                cout << "The only correct argument syntax is --" << arg_str << "=" << endl;
                return false;
            }
        } else {
            cout << "The only acceptable argument is --" << arg_str << "=" << endl;
            return false;
        }
    } else {
        cout << "Arg --" << arg_str << "= not specified!" << endl; 
        return false;
    }
}