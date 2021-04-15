#include <chrono>
#include <fstream>
#include <sstream>
#include <vector>
#include<thread>
#include<mutex>
#include "Dictionary.hpp"
#include "MyHashtable.hpp"

//Tokenize a string into individual word, removing punctuation at the
//end of words
std::vector<std::vector<std::string>> tokenizeLyrics(const std::vector<std::string> files) {
  std::vector<std::vector<std::string>> ret;

  for(auto filename : files) {
    //std::cout<<"reading "<<filename<<"\n";
    std::vector<std::string> my_vect;
    std::ifstream in (filename);

    std::string line;

    //For each line
    while (getline(in, line, '\n')) {
      //Skip all like starting with [
      if (line[0] == '[')
        continue;

      std::stringstream ssline (line);
      //For all words
      while (ssline) {
        std::string word;
        ssline >> word;
        if (ssline) {
          //remove punctuation at the end of word
          while (word.length() > 0
                 && std::ispunct(word[word.length() - 1])) {
            word.pop_back();
          }
          my_vect.push_back(word);
        }
      }
    }
    //std::cout<<"read "<<my_vect.size()<<" words\n";
    ret.push_back(my_vect);
  }
  return ret;
}


void countwords(vector<string> filecontent, HashTable<string, int> &ht, HashTable<string, int> &result)
{
  for (auto &w : filecontent)
  {
    ht.update(w);
  }
  vector<HashNode<string, int>*> entries = ht.getEntries();
  for (auto &entry : entries)
  {
    result.update(entry->getKey(), entry->getValue());
  }
  ht.clear();
}



int main(int argc, char **argv)
{
  if (argc < 4) {
    std::cerr<<"usage: ./main <sources> <testword> <threshold>"<<std::endl;
    return -1;
  }

  // Parse Command Line
  std::string source = argv[1];
  std::string testWord = argv[2];
  int32_t thresholdCount = std::stoi(argv[3]);

  // Obtain List of Files
  std::vector<std::string> files;
  std::ifstream in (source);
  std::string line;
  while (getline(in, line, '\n')) {
    files.push_back(line);
  }

  // Tokenize Lyrics
  auto wordmap = tokenizeLyrics(files);

  HashTable<std::string, int>* ht = new HashTable <std::string, int>[wordmap.size()];
  HashTable<std::string, int> result;

  vector<std::thread> filethreads;
  auto start = chrono::steady_clock::now();
  for (int i = 0; i < wordmap.size(); i++)
  {
    filethreads.push_back(std::thread(countwords, wordmap.at(i), ref(ht[i]), ref(result)));
  }

  for (auto &t : filethreads)
  {
    t.join();
  }

  auto stop = chrono::steady_clock::now();
  chrono::duration<double> time_elapsed = stop - start;


  // Check Hash Table Values 
  /* (you can uncomment, but this must be commented out for tests)
  for (auto it : dict) {
    if (it.second > thresholdCount)
      std::cout << it.first << " " << it.second << std::endl;
  }
  */

  // Do not touch this, need for test cases
  std::cout << result.get(testWord) << std::endl;

  return 0;
}
