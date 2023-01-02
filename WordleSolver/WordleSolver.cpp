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
        line.pop_back();
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
        if ((sugested & wordVec.first) != sugested) continue;
        for (auto& word : wordVec.second)
        {
            bool valid = true;
            for (int i = 0; i < 5; i++)
            {
                if (bannedLetters[i] & 1 << (word[i] - A)) valid = false;
                if (required[i] != 0 && word[i] != required[i]) valid = false;
            }
            if (valid) newList[wordVec.first].push_back(word);
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
    return output;
}

std::string getChoice(std::map<int, std::list<std::string>> &wordList, std::vector<letterChance> &split, int bannedTotal, int bannedLetters[5], char required[5], int sugested)
{
    int idx = 0;
    float score = 0;
    for (auto list : wordList)
    {
        float count = 0;
        for (int i = 0; i < 26; i++) 
        {
            count += (list.first & 1 << i)? split[i].chance : 0;
        }
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
    // std::string file = "C:\\Users\\dande\\source\\repos\\WordleSolver\\WordleSolver\\valid-wordle-words.txt";
    // std::string file = "C:\\Users\\dande\\Source\\Repos\\danderson11235\\WordleSolver\\WordleSolver\\valid-wordle-words.txt";
    std::string file = "/mnt/c/Users/dande/Source/Repos/danderson11235/WordleSolver/WordleSolver/valid-wordle-words.txt";
    std::map<int, std::list<std::string>> wordList = readWordList(file);
    for (int i = 0; i < 6; i++)
    {
        split = getSplit(wordList);
        choice = getChoice(wordList, split, bannedTotal, bannedLetters, requiredLetters, sugested);
        std::cout << "Enter the word [" << choice << "] into wordle" << std::endl;
        std::cout << "enter here x for gray space, y for yellow space, g for green space ->" << std::endl;
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

