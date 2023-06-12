// testtask.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
#include<string>
#include<vector>
#include<map>
#include<unordered_map>
#include <algorithm>

class table 
{
    //имена заголовков
    std::map<std::string,int>columns;
    //данные таблицы
    std::map<std::string, std::vector<std::string>> data;
public:
    table() = default;
    //изменение данных в таблице,проверка и вычисление
    bool changedata(const std::vector<std::string>&);
    //вывод данных
    void out();
private:  
    //вычисление выражений
    bool calculate(const std::vector<std::pair<std::string, int>>&, const std::map<std::string, int>& tcolumns,
        std::map<std::string, std::vector<std::string>>& tdata);
    //функция помогающая в вычислении выражений
    std::pair<int, bool> reccal(const std::string&, std::vector<std::string>, const std::map<std::string, int>& tcolumns,
        std::map<std::string, std::vector<std::string>>& tdata);
    //проверка выражений
    bool check(const std::string &, const std::map<std::string, int>&tcolumns,
    const std::map<std::string, std::vector<std::string>>& tdata);
};

bool table::check(const std::string&str, const std::map<std::string, int> &tcolumns,
const std::map<std::string, std::vector<std::string>> &tdata)
{
    std::string tstr = "";
    std::string temp = "";
    int flag = 0;
    if (str.find('+') != -1)
    { flag++;
    }
    if (str.find('-') != -1)
    {
       flag++;
    }
    if (str.find('/') != -1)
    {
        flag++;
    }
    if (str.find('*') != -1)
    {
        flag++;
    }
    if (flag!=1)
        return false;
    for (int i = 1; i < str.size(); i++) 
    {
        if (str[i] == '+' || str[i] == '/' || str[i] == '-' || str[i] == '*')
        {
            temp = "";
            
            for (int j = tstr.size() - 1; j >= 0 && (tstr[j]>='0' && tstr[j]<='9'); j--)
            {
                temp = tstr[j] + temp;
            }
            
            if ( temp.size() == 0 || tstr.size() != temp.size() && (tdata.find(temp) == tdata.end() || tcolumns.find(tstr.substr(0, tstr.size() - temp.size())) == tcolumns.end()))
            {
                return false;
            }
            tstr.clear();
        }
        else 
        {
            tstr += str[i];
        }
    }
    temp = "";

    for (int j = tstr.size() - 1; j >= 0 && (tstr[j] >= '0' && tstr[j] <= '9'); j--)
    {
        temp = tstr[j] + temp;
    }
    if (temp.size() == 0)
        return false;
    if (tstr.size() == temp.size() && temp == "0" && str.find('/') != -1)
        return false;
    if ( tstr.size() != temp.size() && (tdata.find(temp) == tdata.end() || tcolumns.find(tstr.substr(0, tstr.size() - temp.size())) == tcolumns.end() ))
    {
        return false;
    }
    return true;
}

bool table::changedata(const std::vector<std::string>& strs)
{
    if (strs.empty())
        return false;
    std::map<std::string, int> tcolumns;
    std::map<std::string, std::vector<std::string>> tdata;
    auto str = strs[0];
    if (str.empty() || str[0] != ';' || str[str.size() - 1] == ';')
        return false;
    std::string temp = "";
    std::vector<std::pair<std::string, int>> calstr;
    for (int i = 1,l=0; i < str.size(); i++)
    {
        if (str[i] == ';')
        {
           tcolumns.emplace(temp, l);
            temp.clear();
            l++;
        }
        else
        {
            temp += str[i];
        }
    }

    tcolumns.emplace(temp, tcolumns.size());
    temp.clear();
    for (int i = 1; i < strs.size(); i++)
    {
        str = strs[i];
        int j = 0,check=0;
        std::string key="";
        for (; j < str.size() && str[j] != ';'; j++)
        {
            if (str[0] < '0' || str[0]> '9')
                return false;
            key += str[j];
        }
        if (j == strs.size())
            return false;
        else 
        {
            tdata.emplace(key,std::vector<std::string>());
            j++;
        }
        for (; j < str.size(); j++) {
            if (str[j] < '0' || str[j]> '9' && str[j] != '=')
                return false;
            if ( str[j] >= '0' && str[j] <= '9')
                for (; j < str.size() && str[j] != ';'; j++)
                {
                    if (str[j] < '0' || str[j]> '9')
                        return false;
                    temp += str[j];
                }
            else
            {
                for (; j < str.size() && str[j] != ';'; j++)
                {
                    temp += str[j];
                }
                calstr.push_back(std::pair<std::string,int>(key, check));
            }
            check++;
            if (temp.empty())
                return false;
            tdata[key].push_back(temp);
            temp.clear();
        }
        if (check != tcolumns.size())
            return false;
    }
    for (const auto& par : calstr)
    {
        if(!check(tdata[par.first][par.second],tcolumns,tdata))
        {
            return false;
        }
    }
   if (!calculate(calstr,tcolumns,tdata))
        return false;
    columns=std::move(tcolumns);
    data = std::move(tdata);
   
    return true;
}

std::pair<int,bool> table::reccal(const std::string&str, std::vector<std::string>v, const std::map<std::string, int>& tcolumns,
    std::map<std::string, std::vector<std::string>>& tdata)
{

    std::string temp;
    for (int j = str.size() - 1; j >= 0 && (str[j] >= '0' && str[j] <= '9'); j--)
    {
        temp = str[j] + temp;
    }
    if (temp.size() == str.size())
    {
        return std::pair<int, bool>(stol(temp),true);
    }
    else 
    {
        auto it = tcolumns.find(str.substr(0, str.size() - temp.size()));
        auto tstr = tdata[temp][it->second];
        if (tstr[0] == '=') {
            int i=0, flag = 0;
            if (tstr.find(' +') != -1)
            {
                i = tstr.find(' +'); flag = 1;
            }
            if (tstr.find('-') != -1)
            {
                i = tstr.find(' -'); flag = 2;
            }
            if (tstr.find('/') != -1)
            {
                i = str.find(' /'); flag = 3;
            }
            if (tstr.find('*') != -1)
            {
                i = tstr.find(' *'); flag = 4;
            }

            std:: string temp1 = tstr.substr(1, i - 1);
            if (std::find(v.begin(), v.end(), temp1) != v.end())
                return std::pair<int, bool>(0, false);
            v.push_back(temp1);

            auto it1 = reccal(temp1,v,tcolumns,tdata);
            if (!it1.second)
                return std::pair<int, bool>(0, false);
            v.pop_back();
            temp1= tstr.substr(i + 1, tstr.size() - i - 1);
            if (std::find(v.begin(), v.end(), temp1) != v.end())
                return std::pair<int, bool>(0, false);
            v.push_back(temp1);
            auto it2 = reccal(temp1, v,tcolumns,tdata); 
            if (!it2.second)
                 return std::pair<int, bool>(0, false);
            switch (flag)
            {
            case 1:
                tdata[temp][it->second] = std::to_string(it1.first + it2.first); 
                return std::pair<int, bool>(it1.first + it2.first, true); 
            case 2:
                tdata[temp][it->second] = std::to_string(it1.first - it2.first); 
                return std::pair<int, bool>(it1.first + it2.first, true); 
            case 3:tdata[temp][it->second] = std::to_string(it1.first / it2.first); 
                return std::pair<int, bool>(it1.first + it2.first, true);
            case 4:tdata[temp][it->second] = std::to_string(it1.first * it2.first); 
                return std::pair<int, bool>(it1.first + it2.first, true); 

            }
        }
        else 
        {
            return std::pair<int, bool>(stol(tstr), true);
        }
    }
    return std::pair<int, bool>(0, false);
}


bool table::calculate(const std::vector<std::pair<std::string, int>>& vec, const std::map<std::string, int>& tcolumns,
    std::map<std::string, std::vector<std::string>>& tdata)
{
    std::string temp = "";
    for (const auto& par : vec) {
        auto str = tdata[par.first][par.second];
        if (str[0] == '=') {
            int i=0, flag = 0;
            if (str.find(' +') != -1)
            {
                i = str.find(' +'); flag = 1;
            }
            if (str.find('-') != -1)
            {
                i = str.find(' -'); flag = 2;
            }
            if (str.find('/') != -1)
            {
                i = str.find(' /'); flag = 3;
        }
            if (str.find('*') != -1)
            {
                i = str.find(' *'); flag = 4;
            }
            std::vector<std::string> v;
            temp = str.substr(1, i - 1);
           auto it1 = reccal(temp, v,tcolumns,tdata);
            if (!it1.second)
                return false;
            temp = str.substr(i + 1, str.size() - i - 1);
            auto it2 = reccal(temp, v,tcolumns,tdata);
            if (!it2.second)
                return false;
            switch (flag) 
            {
            case 1:tdata[par.first][par.second] = std::to_string(it1.first + it2.first); break;
            case 2:tdata[par.first][par.second] = std::to_string(it1.first - it2.first); break;
            case 3:tdata[par.first][par.second] = std::to_string(it1.first / it2.first); break;
            case 4:tdata[par.first][par.second] = std::to_string(it1.first * it2.first); break;

            }
            
        }
    
}
    return true;
}

void table::out() 
{
    if (columns.empty() || data.empty())
        return;
    for (const auto& str : columns)
    {
        std::cout << ',' << str.first;
    }
    std::cout << "\n";
    for (const auto & iter:data) 
    {
        std::cout << iter.first;
        for (const auto& str : iter.second)
        {
            std::cout << ',' << str;
        }
        std::cout << "\n";
    }
}

int main()
{ 
    std::string str;
    std::cout << "enter the file name \n";
    std::cin >> str;
    if (str.size() < 5 || (str.substr(str.size() - 4, 4) != ".csv"))
        exit(1);
    std::ifstream  text(str);
    if (!text.is_open())
        std::exit(1);
    std::vector<std::string> vec;
   table tab;
    while (text>> str)
    { 
        vec.push_back(str);
    } 
    text.close();
    if(!tab.changedata(vec))
        std::exit(1);
    tab.out();
}
