#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<map>
#include<vector>
#include<algorithm>

// все, что помечается (optional) может быть удалено
// возвращает true, если слово содержит хоть одну неизмененную в соответствие с грамматикой литеру
bool let_in_rules(std::string word, std::map<std::string, std::vector<std::string>> rules_dict) {
	bool flag = false;
	for (int i = 0; i < word.length(); i++) {
		for (auto itr = rules_dict.begin(); itr != rules_dict.end(); ++itr) {
			std::string let = itr->first;
			std::stringstream ss;
			std::string s;
			ss << word[i];
			ss >> s;
			if (s == let) {
				flag = true;
			}
		}
	}
	return flag;
}

// считает количество возможных комбинаций с учетом длины слова и всех грамматик
int count_comb(std::map<std::string, std::vector<std::string>> rules_dict, std::string word)
{
	int count = 0;

	for (auto itr = rules_dict.begin(); itr != rules_dict.end(); ++itr) {
		for (int i = 0; i < word.length(); i++) {
			std::stringstream ss;
			std::string let;
			ss << word[i];
			ss >> let;
			if (itr->first == let) {
				count += itr->second.size();
			}
		}
	}
	return count;
}

// вывод вектора (optional)
void output_vec(std::vector<std::string> vec)
{
	for (auto& i : vec) {
		std::cout << i << " ";
	}
	std::cout << "\n";
}

// вывод словаря (optional)
void output_dict(std::map<std::string, std::vector<std::string>> rules_dict)
{
	std::cout << "KEY\tELEMENT\n";
	for (auto itr = rules_dict.begin(); itr != rules_dict.end(); ++itr) {
		std::cout << itr->first << '\t';
		output_vec(itr->second);
	}
}

// функция настроек сортировки
bool cmp(std::pair<std::string, std::vector<std::string>>& a,
	std::pair<std::string, std::vector<std::string>>& b)
{
	return a.second.size() > b.second.size();
}

// позволяет отсортировать map и обращаться по []
std::pair<std::string, std::vector<std::string>> get_it_sort(std::map<std::string, std::vector<std::string>>& M, int pos)
{
	std::vector<std::pair<std::string, std::vector<std::string>>> vec;
	for (auto& x : M) {
		vec.push_back(x);
	}
	std::sort(vec.begin(), vec.end(), cmp);

	int i = 0;
	for (auto x : vec) {
		if (i == pos) {
			return x;
		}
		i++;
	}
}

// меняет в строке str строку value1 на value2, если нечего менять, возвращает str
std::string replace(std::string str, std::string value1, std::string value2) 
{
	std::size_t pos = str.find(value1);
	std::string new_str = "";
	if (pos != std::string::npos) {
		new_str += str.substr(0, pos);
		new_str += value2;
		new_str += str.substr(pos + 1, str.length());
		return new_str;
	}
	return str;
}

// парсинг грамматик
std::vector<std::string> parse(std::string rule, std::string delimiter = "|")
{
	std::vector<std::string> vec;
	size_t pos = 0;
	std::string token;

	while ((pos = rule.find(delimiter)) != std::string::npos) {
		token = rule.substr(0, pos);
		vec.push_back(token);
		rule.erase(0, pos + delimiter.length());
	}
	vec.push_back(rule);
	return vec;
}

// алгоритм праволинейной грамматики
void right_linear(int len, std::string word, std::map<std::string, std::vector<std::string>> rules)
{
	int n_comb = count_comb(rules, word);
	std::string* result = new std::string[n_comb];
	std::string* tmp_str = new std::string[n_comb];
	
	std::pair<std::string, std::vector<std::string>>* arr = new std::pair<std::string, std::vector<std::string>>[len];

	for (int i = 0; i < len; i++) {
		arr[i] = get_it_sort(rules, i);
	}

	for (int i = 0; i < n_comb; i++) {
		result[i] = "S=>" + word;
		tmp_str[i] = word;
	}

	bool flag = true;
	for (int i = 0; i < n_comb; i++) {
		for (int j = 0; j < len; j++) {
			tmp_str[i] = replace(tmp_str[i], arr[j].first, arr[j].second[i % arr[j].second.size()]);
			result[i] = result[i] + "=>" + tmp_str[i];
		}

		flag = let_in_rules(tmp_str[i], rules);
		if (flag) {
			result[i] = result[i].substr(0, result[i].length() - tmp_str[i].length() - 2);
			i--;
		}
	}

	for (int i = 0; i < n_comb; i++) {
		std::cout << result[i] << "\n";
	}
}

// мейн функция
int main()
{
	std::ifstream fin{ "../rules.txt" }; // чтение файла с грамматикой (путь указать свой)
	std::vector<std::string> rules;
	std::string rule;
	while (fin >> rule) {
		rules.push_back(rule);
	}

	const int RULES_COUNT = rules.size();

	std::map<std::string, std::vector<std::string>> rules_dict;

	for (const std::string& rule : rules) {
		std::pair<std::string, std::vector<std::string>> pr; 
		pr.first = rule[0];
		pr.second = parse(rule.substr(3, rule.length()));
		rules_dict.insert(pr);
		//std::cout << pr.first << " -> "; // (optional)
		//output_vec(pr.second);
	}
	
	output_dict(rules_dict); // (optional)

	std::string word;
	std::cout << "Input word: ";
	std::cin >> word;
	std::cout << "\n";

	right_linear(RULES_COUNT, word, rules_dict);

	return 0;
}