// WordleSolver.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <map>

char A = 'a';

struct letterChance
{
    char letter;
    float chance;
    bool operator<(const letterChance& b) const { return (chance < b.chance); }
};

std::string print(std::vector<letterChance> list)
{
    std::stringstream output;
    for (auto& letter : list) output << "[" << letter.letter << ", " + std::to_string(letter.chance) << "]";
    return output.str();
}

int getWordValue(std::string word)
{
    int output = 0;
    for (int i = 0; i < word.length(); i++)
    {
        output |= 1 << (word[i] - A);
    }
    return output;
}

int countBitsSet(int x)
{
    int output = 0;
    for (int i = 0; i < 32; i++)
    {
        output += x & 1;
        x = x >> 1;
    }
    return output;
}

std::map<int, std::list<std::string>> readWordList(std::string &fileName) 
{
    std::map<int, std::list<std::string>> output;
    std::ifstream file;
    file.open(fileName);
    std::string line;
    while (file)
    {
        std::getline(file, line);
        output[getWordValue(line)].push_back(line);
    }
    return output;
}

std::map<int, std::list<std::string>> getNewWordList(std::map<int, std::list<std::string>> &oldList, int bannedTotal, int bannedLetters[5], char required[5], int sugested)
{
    std::map<int, std::list<std::string>> newList;
    for (auto& wordVec : oldList)
    {
        if (bannedTotal & wordVec.first) continue;
        for (auto& word : wordVec.second)
        {
            for (int i = 0; i < 5; i++)
            {
                if (bannedLetters[i] & 1 << (word[i] - A)) continue;
                if (word[i] != required[i]) continue;
                if ((wordVec.first & sugested) != sugested) continue;
            }
            newList[wordVec.first].push_back(word);
        }
    }
    return newList;
}

std::vector<letterChance> getSplit(std::map<int, std::list<std::string>> &wordList)
{
    float count[26] = {0};
    std::vector<letterChance> output(26);
    float total = 0;
    for (auto &wordVec : wordList)
    {
        for (auto &word : wordVec.second)
        {
            for (int i = 0; i < 26; i++)
            {
                count[i] += (bool)((1 << i) & getWordValue(word));
            }
            total++;
        }
    }
    for (int i = 0; i < 26; i++) output[i] = letterChance{ (char)(i + A), 1.0f - std::abs((count[i] / total) - 0.5f) };
    std::sort(output.begin(), output.end());
    std::reverse(output.begin(), output.end());
    return output;
}

std::string getChoice(std::map<int, std::list<std::string>> &wordList, std::vector<letterChance> &split, int bannedTotal, int bannedLetters[5], char required[5], int sugested)
{
    std::string word = "";
    int idx = 0;
    for (int i = 0; i < 5; i++)
    {
        if (required[i] != 0)
        {
            word += required[i];
            continue;
        }
        word += split[idx++].letter;
    }
    int wordval = getWordValue(word);
    idx = 0;
    int score = 0;
    for (auto list : wordList)
    {
        int count = countBitsSet(list.first & wordval);
        if (count < score) continue;
        idx = list.first;
        score = count;
    }
    if (score == 0) return "";
    return wordList[idx].front();
}

void parseResult(std::string word, std::string result, int& bannedTotal, int bannedLetters[5], char required[5], int& sugested)
{
    // if letter green
    for (int i = 0; i < 5; i++)
    {
        if (result[i] == 'g')
        {
            sugested |= 1 << (word[i] - A);
            required[i] = word[i];
            bannedTotal &= ~(1 << (word[i] - A));
        }
        else if (result[i] == 'y')
        {
            sugested |= 1 << (word[i] - A);
            bannedLetters[i] |= 1 << (word[i] - A);
            bannedTotal &= ~(1 << (word[i] - A));
        }
        else if (result[i] == 'x')
        {
            if (!(sugested & 1 << (word[i] - A))) bannedTotal |= 1 << (word[i] - A);
        }
    }
    return;
}

int main()
{
    std::cout << "Welcome to wordle solver, initializing data structures" << std::endl;
    int bannedTotal = 0;
    int sugested = 0;
    int bannedLetters[5] = { 0 };
    char requiredLetters[5] = { 0 };
    std::vector<letterChance> split;
    std::string choice;
    std::string result;
    std::string file = "C:\\Users\\dande\\source\\repos\\WordleSolver\\WordleSolver\\valid-wordle-words.txt";
    std::map<int, std::list<std::string>> wordList = readWordList(file);
    for (int i = 0; i < 6; i++)
    {
        split = getSplit(wordList);
        choice = getChoice(wordList, split, bannedTotal, bannedLetters, requiredLetters, sugested);
        std::cout << "Enter the word " << choice << " into wordle\nenter here x for gray space, y for yellow space, g for green space ->" << std::endl;
        if (choice == "") break;
        std::cin >> result;
        if (result == "ggggg")
        {
            std::cout << "Nice job we got it" << std::endl;
            return 0;
        }
        parseResult(choice, result, bannedTotal, bannedLetters, requiredLetters, sugested);
        wordList = getNewWordList(wordList, bannedTotal, bannedLetters, requiredLetters, sugested);
    }

    std::cout << "Oh no, blame dave that I didn't work" << std::endl;
    return 1;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
