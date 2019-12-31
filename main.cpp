//
//  main.cpp
//  project3_redo
//
//  Created by 益佳宇 on 2018/11/16.
//  Copyright © 2018年 益佳宇. All rights reserved.
//

#include <iostream>
#include "TableEntry.h"
#include <unordered_map>
#include <map>
#include <vector>
#include <getopt.h>
#include <algorithm>
using namespace std;
using std::cin;
using std::cout;
using std::boolalpha;
enum class index_status {No_idx, Hash_idx, Bst_idx};
bool if_bigger (const TableEntry &a, const TableEntry &b){
    return (a > b);
}
bool if_smaller (const TableEntry &a, const TableEntry &b){
    return (a < b);
}
bool if_equal (const TableEntry &a, const TableEntry &b){
    return (a == b);
}

class functor{
    TableEntry value;
    size_t idx_col;
    char op;
public:
    functor(const TableEntry& v, const size_t &col, const char &p):value(v), idx_col(col),op(p){};
    bool operator()(const vector<TableEntry> &row){
        switch(op){
            case '=':
                return (row[idx_col] == value);
                break;
            case '>':
                return (row[idx_col] > value);
                break;
            case '<':
                return (row[idx_col] < value);
                break;
            default:
                return (row[idx_col] == value);
                break;
        }
    }
};

class Table{
public:
    size_t colN = 0;
    size_t rowN = 0;
    vector<EntryType> types;
    unordered_map<string,size_t> colnames;
    vector<vector<TableEntry>> content;
    unordered_map<TableEntry, vector<TableEntry*>> u_m;
    map<TableEntry, vector<TableEntry*>> m;
    index_status idx_status = index_status::No_idx;
    size_t u_m_idx = 0;
    size_t m_idx = 0;
    string tablename;
    void create(string name){
        string command;
        tablename = name;
        cin >> colN;
        types.reserve(colN);
        colnames.reserve(colN);
        for (size_t i=0; i<colN; i++){
            cin >> command;
            if (command == "string"){
                types.push_back(EntryType::String);
            }
            else if (command == "double"){
                types.push_back(EntryType::Double);
            }
            else if (command == "int"){
                types.push_back(EntryType::Int);
            }
            else if (command == "bool"){
                types.push_back(EntryType::Bool);
            }
        }
        cout << "New table " << tablename << " with column(s) ";
        for (size_t i=0; i<colN; i++){
            cin >> command;
            colnames.insert(make_pair(command,i));
            cout << command << " ";
        }
        cout << "created\n";
    }
    
    void insert_helper(vector<TableEntry> &line){
        for (size_t j=0; j<colN; j++){
            EntryType x = types[j];
            switch(x){
                case EntryType::String:{
                    string a;
                    cin >> a;
                    line.emplace_back(a);
                    break;
                }
                case EntryType::Double:{
                    double a;
                    cin >> a;
                    line.emplace_back(a);
                    break;
                }
                case EntryType::Bool:{
                    bool a;
                    cin >> a;
                    line.emplace_back(a);
                    break;
                }
                case EntryType::Int:{
                    int a;
                    cin >> a;
                    line.emplace_back(a);
                    break;
                }
            }
        }
    }
    void insert(){
        size_t N;//加入的行数
        cin >> N;
        string command;
        cin >> command;//get ROWS
        if (idx_status == index_status::Hash_idx){
            for (size_t i=0;i<N;i++){
                vector<TableEntry> line;
                line.reserve(colN);
                insert_helper(line);
                content.push_back(line);
                rowN++;
                vector<TableEntry*> pass_in;
                auto p = content[rowN-1].data();
                pass_in.push_back(p);
                if (!u_m.insert(make_pair(content[rowN-1][u_m_idx],pass_in)).second){
                    u_m[content[rowN-1][u_m_idx]].push_back(p);
                }
            }
        }
        else if (idx_status == index_status::Bst_idx){
            for (size_t i=0;i<N;i++){
                vector<TableEntry> line;
                line.reserve(colN);
                insert_helper(line);
                content.push_back(line);
                rowN++;
                vector<TableEntry*> pass_in;
                auto p = content[rowN-1].data();
                pass_in.push_back(p);
                if (!m.insert(make_pair(content[rowN-1][m_idx],pass_in)).second){
                    m[content[rowN-1][m_idx]].push_back(p);
                }
            }
        }
        else{
            for (size_t i=0; i<N; i++){
                vector<TableEntry> line;
                line.reserve(colN);
                insert_helper(line);
                content.push_back(line);
                rowN++;
            }
        }
        cout << "Added " << N << " rows to " << tablename << " from position " << rowN-N << " to " << rowN-1 << '\n';
    }
    void del(){
        string command;
        cin >> command;
        cin >> command;
        auto it = colnames.find(command);
        if (it == colnames.end()){
            cout << "Error: " << command << " does not name a column in " << tablename <<'\n';
            getline(cin,command);
            return;
        }
        char op;
        cin >> op;
        size_t col = it->second;
        EntryType x = types[col];
        switch(x){
            case EntryType::String:{
                string value;
                cin >> value;
                TableEntry v(value);
                del_helper(op, v, col);
                break;
            }
            case EntryType::Double:{
                double value;
                cin >> value;
                TableEntry v(value);
                del_helper(op, v, col);
                break;
            }
            case EntryType::Int:{
                int value;
                cin >> value;
                TableEntry v(value);
                del_helper(op,v,col);
                break;
            }
            case EntryType::Bool:{
                bool value;
                cin >> value;
                TableEntry v(value);
                del_helper(op,v,col);
                break;
            }
        }
    }
    
    void del_helper(const char &op, const TableEntry &v,const size_t &col){
        size_t deleted = 0;
        vector<vector<TableEntry>>::iterator it;
        functor compare(v,col,op);
        it = remove_if(content.begin(), content.end(), compare);
        deleted = (size_t)distance(it,content.end());
        content.erase(it,content.end());
        rowN -= deleted;
        if (idx_status == index_status::Hash_idx){
            generate_hash_helper();
        }
        else if (idx_status == index_status::Bst_idx){
            generate_bst_helper();
        }
        cout << "Deleted " << deleted << " rows from " << tablename <<'\n';
    }

    void print(bool if_quiet){
        size_t num;
        cin >> num;
        vector<size_t> to_print;
        to_print.reserve(num);
        string temp;
        vector<string> col_to_print;
        col_to_print.reserve(num);
        for (size_t i=0; i<num; i++){
            cin >> temp;
            if (colnames.find(temp) != colnames.end()){
                to_print.push_back(colnames[temp]);
                col_to_print.push_back(temp);
            }
            else{
                cout << "Error: " << temp << " does not name a column in " << tablename <<'\n';
                getline(cin,temp);
                return;
            }
        }
        cin >> temp;//Get WHERE or ALL
        if (temp == "ALL"){
            if (if_quiet == true)
                cout <<  "Printed " << rowN << " matching rows from " << tablename <<'\n';
            else{
                if (!if_quiet){
                    for (size_t i=0; i<num; i++){
                        cout << col_to_print[i] << " ";
                    }
                    cout << '\n';
                }
                for (size_t i=0; i<rowN; i++){
                    for (size_t j=0; j<num;j++){
                        cout << content[i][to_print[j]] << " ";
                    }
                    cout << '\n';
                }
                cout << "Printed " << rowN << " matching rows from " << tablename <<'\n';
            }
        }
        else{
            cin >> temp;//temp is a colname;
            size_t temp_idx;
            if (colnames.find(temp) != colnames.end()){
                temp_idx = colnames[temp];
            }
            else{
                cout << "Error: " << temp << " does not name a column in " << tablename <<'\n';
                getline(cin,temp);
                return;
            }
            if (!if_quiet){
                for (size_t i=0; i<num; i++){
                    cout << col_to_print[i] << " ";
                }
                cout << '\n';
            }
            char op;
            cin >> op;
            EntryType x = types[temp_idx];
            switch(x){
                case EntryType::String:{
                    string value;
                    cin >> value;
                    TableEntry v(value);
                    print_helper(op, v, temp_idx, to_print, num, if_quiet);
                    break;
                }
                case EntryType::Bool:{
                    bool value;
                    cin >> value;
                    TableEntry v(value);
                    print_helper(op, v, temp_idx, to_print, num,if_quiet);
                    break;
                }
                case EntryType::Double:{
                    double value;
                    cin >> value;
                    TableEntry v(value);
                    print_helper(op, v, temp_idx, to_print, num,if_quiet);
                    break;
                }
                case EntryType::Int:{
                    int value;
                    cin >> value;
                    TableEntry v(value);
                    print_helper(op, v, temp_idx, to_print, num,if_quiet);
                    break;
                }
            }
        }
    }
    void print_helper(char &op, const TableEntry &v,const size_t &col,vector<size_t> &print_col,size_t &num,bool if_q){
        size_t printed = 0;
        if (idx_status == index_status::Bst_idx && col == m_idx){
            map<TableEntry, vector<TableEntry*>>::iterator first,last;
            switch (op) {
                case '=':
                    first = m.equal_range(v).first;
                    last = m.equal_range(v).second;
                    break;
                case '>':
                    first = m.upper_bound(v);
                    last = m.end();
                    break;
                case '<':
                    first = m.begin();
                    last = m.lower_bound(v);
                    break;
            }
            for (auto it = first; it !=last; it++){
                size_t vec_size = it->second.size();
                printed += vec_size;
                if (!if_q){
                    for (size_t t=0; t<vec_size; t++){
                        for (size_t j=0; j<num; j++){
                            cout << *(it->second[t]+print_col[j]) << " ";
                        }
                        cout << '\n';
                    }
                }
            }
        }
        else if (idx_status == index_status::Hash_idx && col == u_m_idx && op == '='){
            auto it = u_m.find(v);
            if (it != u_m.end()){
                size_t vec_size = it->second.size();
                printed += vec_size;
                if (!if_q){
                    for (size_t t=0; t<vec_size; t++){
                        for (size_t j=0; j<num; j++){
                            cout << *(it->second[t]+print_col[j]) << " ";
                        }
                        cout << '\n';
                    }
                }
            }
        }
        else{
            bool (*p)(const TableEntry &a, const TableEntry &b);
            switch (op) {
                case '=':
                    p = if_equal;
                    break;
                case '>':
                    p = if_bigger;
                    break;
                case '<':
                    p = if_smaller;
                    break;
                default:
                    p = if_equal;
                    break;
            }
            for (size_t i=0; i<rowN; i++){
                if (p(content[i][col],v)){
                    if (!if_q){
                        for (size_t j=0; j<num;j++){
                            cout << content[i][print_col[j]] << " ";
                        }
                        cout << '\n';
                    }
                    printed++;
                }
            }
        }
        cout << "Printed " << printed << " matching rows from " << tablename <<'\n';
    }
    
    void generate_hash(){
        idx_status = index_status::Hash_idx;
        string temp;
        cin >> temp;
        cin >> temp;
        cin >> temp;
        if (colnames.find(temp) != colnames.end()){
            u_m_idx = colnames[temp];
        }
        else{
            cout << "Error: " << temp << " does not name a column in " << tablename <<'\n';
            getline(cin,temp);
            return;
        }
        generate_hash_helper();
        cout << "Created hash index for table " << tablename << " on column " << temp <<'\n';
    }
    
    void generate_hash_helper(){
        u_m.clear();
        for (size_t i=0; i<rowN; i++){
            vector<TableEntry*> pass_in;
            auto p = content[i].data();
            pass_in.push_back(p);
            if (!u_m.insert(make_pair(content[i][u_m_idx],pass_in)).second){
                u_m[content[i][u_m_idx]].push_back(p);
            }
        }
    }
    
    void generate_bst(){
        idx_status = index_status::Bst_idx;
        string temp;
        cin >> temp;
        cin >> temp;
        cin >> temp;
        if (colnames.find(temp) != colnames.end()){
            m_idx = colnames[temp];
        }
        else{
            cout << "Error: " << temp << " does not name a column in " << tablename <<'\n';
            getline(cin,temp);
            return;
        }
        generate_bst_helper();
        cout << "Created bst index for table " << tablename << " on column " << temp <<'\n';
    }
    
    void generate_bst_helper(){
        m.clear();
        for (size_t i=0; i<rowN; i++){
            vector<TableEntry*> pass_in;
            auto p = content[i].data();
            pass_in.push_back(p);
            if (!m.insert(make_pair(content[i][m_idx],pass_in)).second){
                m[content[i][m_idx]].push_back(p);
            }
        }
    }
};
int main(int argc, char *argv[]) {
    ios_base::sync_with_stdio(false);
    cin >> boolalpha;
    cout << boolalpha;
    int option_index = 0;
    int choice;
    bool if_quiet = false;
    option long_options[] = {
        {"help",no_argument,nullptr,'h'},
        {"quiet",required_argument,nullptr,'q'},
        { nullptr, 0,                 nullptr, '\0' }
    };
    while ((choice = getopt_long(argc, argv, "hq", long_options, &option_index)) != -1){
        switch(choice){
            case 'h':
                cout << "Help o help!\n";
                exit(0);
                break;
            case 'q':
                if_quiet = true;
                break;
        }
    }
#ifdef __APPLE__
    string infile = "/Users/helen/Documents/UM/281/xcode/project3_redo/project3_redo/test-5-table-commands.txt";
    if (!freopen(infile.c_str(),"r",stdin)){
        cerr<<"Unable to open input file"<<"\n";
        exit(1);
    }
    //string outfile = "/Users/helen/Documents/UM/281/xcode/project3_redo/project3_redo/long-help.txt";
    //freopen(outfile.c_str(),"w",stdout);
#endif
    string operation,command;
    unordered_map<string, Table*> database;
    do{
        cout << "% " << flush;
        cin >> operation;
        if (operation == "CREATE"){
            cin >> command;
            if (database.find(command) != database.end()){
                cout << "Error: Cannot create already existing table " << command <<"\n";
                getline(cin,command);
                continue;
            }
            Table *p = new Table;
            p->create(command);
            database[p->tablename] = p;
        }
        else if (operation == "INSERT"){
            cin >> operation;
            cin >> command;
            if (database.find(command) == database.end()){
                cout << "Error: " << command << " does not name a table in the database\n";
                size_t N_error;
                cin >> N_error;
                getline(cin,command);
                for (size_t i=0; i<N_error; i++){
                    getline(cin,command);
                }
                continue;
            }
            Table *temp = database[command];
            temp->insert();
        }
        else if (operation == "DELETE"){
            cin >> command;
            cin >> command;
            if (database.find(command) == database.end()){
                cout << "Error: " << command << " does not name a table in the database\n";
                getline(cin,command);
                continue;
            }
            Table *temp = database[command];
            temp->del();
        }
        else if (operation[0] == '#'){
            getline(cin,operation);
            //cout << '\n';
        }
        else if (operation == "REMOVE"){
            cin >> command;
            if (database.find(command) == database.end()){
                cout << "Error: " << command << " does not name a table in the database\n";
                getline(cin,command);
                continue;
            }
            Table* victim = database.find(command)->second;
            database.erase(command);
            delete victim;
            cout << "Table " << command << " deleted\n";
        }
        else if (operation == "PRINT"){
            cin >> command;//take in FROM
            cin >> command;
            if (database.find(command) == database.end()){
                cout << "Error: " << command << " does not name a table in the database\n";
                getline(cin,command);
                continue;
            }
            Table *temp = database[command];
            temp->print(if_quiet);
        }
        else if (operation == "GENERATE"){
            cin >> command;//Get FOR
            cin >> command;//Get tablename
            if (database.find(command) == database.end()){
                cout << "Error: " << command << " does not name a table in the database\n";
                getline(cin,command);
                continue;
            }
            cin >> operation;//Get indextype
            Table *temp = database[command];
            if (operation == "hash"){
                temp->generate_hash();
            }
            else if (operation == "bst"){
                temp->generate_bst();
            }
        }
        else if (operation == "JOIN"){
            size_t rows_printed = 0;
            bool error = false;
            cin >> command;
            if (database.find(command) == database.end()){
                cout << "Error: " << command << " does not name a table in the database\n";
                getline(cin,command);
                continue;
            }
            Table *t1 = database[command];
            cin >> command;//Take in AND
            cin >> command;
            if (database.find(command) == database.end()){
                cout << "Error: " << command << " does not name a table in the database\n";
                getline(cin,command);
                continue;
            }
            Table *t2 = database[command];
            cin >> command;//WHERE
            cin >> command;//column1
            if (t1->colnames.find(command) == t1->colnames.end()){
                cout << "Error: " << command << " does not name a column in " << t1->tablename <<'\n';
                getline(cin,command);
                continue;
            }
            size_t col_1 = t1->colnames[command];
            cin >> command;//op
            cin >> command;//column2;
            if (t2->colnames.find(command) == t2->colnames.end()){
                cout << "Error: " << command << " does not name a column in " << t2->tablename <<'\n';
                getline(cin,command);
                continue;
            }
            size_t col_2 = t2->colnames[command];
            cin >> command >> command;//and print
            size_t N;
            cin >> N;
            struct join_col{
                size_t col_idx;
                bool which;//说明是打印table1的内容还是table2的内容，true表示table1， false表示table2
            };
            vector<join_col> to_print;
            to_print.reserve(N);
            vector<string> to_print_colname;
            to_print_colname.reserve(N);
            for (size_t i=0; i<N; i++){
                cin >> command;//print_colname
                size_t uni;//indicate from 1 or 2;
                cin >> uni;
                if (uni == 1){
                    join_col temp;
                    if (t1->colnames.find(command) == t1->colnames.end()){
                        cout << "Error: " << command << " does not name a column in " << t1->tablename <<'\n';
                        getline(cin,command);
                        error = true;
                        break;
                    }
                    temp.col_idx = t1->colnames[command];
                    temp.which = true;
                    to_print.push_back(temp);
                }
                else{
                    join_col temp;
                    if (t2->colnames.find(command) == t2->colnames.end()){
                        cout << "Error: " << command << " does not name a column in " << t2->tablename <<'\n';
                        getline(cin,command);
                        error = true;
                        break;
                    }
                    temp.which = false;
                    temp.col_idx = t2->colnames[command];
                    to_print.push_back(temp);
                }
                to_print_colname.push_back(command);
            }
            if (error == true)
                continue;
            if (!if_quiet){
                for (size_t i=0; i<N; i++){
                    cout << to_print_colname[i] << " ";
                }
                cout << '\n';
            }
            if (t2->idx_status == index_status::Bst_idx && col_2 == t2->m_idx){
                for (size_t i=0; i<t1->rowN; i++){
                    auto it = t2->m.find(t1->content[i][col_1]);
                    if (it != t2->m.end()){
                        size_t it_size = it->second.size();
                        rows_printed += it_size;
                        if (!if_quiet){
                            for (size_t j=0; j<it_size; j++){
                                for (size_t m=0; m<N; m++){
                                    if (to_print[m].which == 1){
                                        cout << t1->content[i][to_print[m].col_idx] << " ";
                                    }
                                    else
                                        cout << *(it->second[j]+to_print[m].col_idx) << " ";
                                }
                                cout << '\n';
                            }
                        }
                    }
                }
            }
            else if (t2->idx_status == index_status::Hash_idx && col_2 == t2->u_m_idx){
                for (size_t i=0; i<t1->rowN; i++){
                    auto it = t2->u_m.find(t1->content[i][col_1]);
                    if (it != t2->u_m.end()){
                        size_t it_size = it->second.size();
                        rows_printed += it_size;
                        if (!if_quiet){
                            for (size_t j=0; j<it_size; j++){
                                for (size_t m=0; m<N; m++){
                                    if (to_print[m].which == 1){
                                        cout << t1->content[i][to_print[m].col_idx] << " ";
                                    }
                                    else
                                        cout << *(it->second[j]+to_print[m].col_idx) << " ";
                                }
                                cout << '\n';
                            }
                        }
                    }
                }
            }
            else{
                unordered_map<TableEntry,vector<TableEntry*>> join;//key是名字，value是行数
                for (size_t i=0; i<t2->rowN; i++){
                    vector<TableEntry*> temp_v;
                    auto p = t2->content[i].data();
                    temp_v.push_back(p);
                    if (join.find(t2->content[i][col_2]) == join.end()){
                        join[t2->content[i][col_2]] = temp_v;
                    }
                    else{
                        join[t2->content[i][col_2]].push_back(p);
                    }
                }
                for (size_t i=0; i<t1->rowN; i++){
                    auto it = join.find(t1->content[i][col_1]);
                    if (it != join.end()){
                        size_t it_size = it->second.size();
                        rows_printed += it_size;
                        if (!if_quiet){
                            for (size_t j=0; j<it_size; j++){
                                for (size_t m=0; m<N; m++){
                                    if (to_print[m].which == 1){
                                        cout << t1->content[i][to_print[m].col_idx] << " ";
                                    }
                                    else
                                        cout << *(it->second[j]+to_print[m].col_idx) << " ";
                                }
                                cout << '\n';
                            }
                        }
                    }
                }
            }
            cout << "Printed " << rows_printed << " rows from joining " << t1->tablename << " to " << t2->tablename << '\n';
        }
        else if (operation == "QUIT")
            break;
        else{
            cout << "Error: unrecognized command\n";
            string garbage;
            getline(cin,garbage);
        }
    }while(!cin.eof());
    for (auto it = database.begin(); it != database.end(); it++){
        Table* vic;
        vic = it->second;
        delete vic;
    }
    cout << "Thanks for being silly!\n";
    return 0;
}
//改一下upper bound 和 lower bound的问题
//改一下hash table 的问题
//Delete remove_if
//Print_helper hash
