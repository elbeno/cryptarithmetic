#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

auto remove_nonalpha(const string& s)
{
  string out;
  copy_if(s.cbegin(), s.cend(), back_inserter(out),
          [] (char c) { return isalpha(c); });
  transform(out.begin(), out.end(), out.begin(),
            [] (char c) { return tolower(c); });
  return out;
}

auto get_words(const string& dict_filename)
{
  vector<string> words;
  ifstream f(dict_filename);
  string line;
  while (getline(f, line))
  {
    words.emplace_back(remove_nonalpha(line));
  }
  sort(words.begin(), words.end());
  words.erase(unique(words.begin(), words.end()), words.end());
  return words;
}

auto letters_from_words(string& w1, string& w2)
{
  // Check words are well-formed
  transform(w1.cbegin(), w1.cend(), w1.end(),
            [] (char c) { return tolower(c); });
  transform(w2.cbegin(), w2.cend(), w2.end(),
            [] (char c) { return tolower(c); });
  if (!all_of(w1.cbegin(), w1.cend(),
              [] (char c) { return c >= 'a' && c <= 'z'; }))
  {
    cout << "Word 1 (" << w1 << ") has non-alphabetic characters." << endl;
    exit(2);
  }
  if (!all_of(w2.cbegin(), w2.cend(),
              [] (char c) { return c >= 'a' && c <= 'z'; }))
  {
    cout << "Word 2 (" << w2 << ") has non-alphabetic characters." << endl;
    exit(3);
  }

  // Check we don't have too many letters
  auto letters = w1 + w2;
  sort(letters.begin(), letters.end());
  letters.erase(unique(letters.begin(), letters.end()), letters.end());
  if (letters.size() > 10)
  {
    cout << "Supplied words (" << w1 << ", " << w2
         << ") have too many distinct letters." << endl;
    exit(4);
  }
  // Expand to 10 letters
  auto i = letters.end();
  letters.resize(10);
  iota(i, letters.end(), '0');
  sort(letters.begin(), letters.end());
  return letters;
}

int string_to_int(const string& s, const string& lettermap)
{
  int val = 0;
  for (auto i = s.cbegin(); i != s.cend(); ++i)
  {
    val *= 10;
    val += lettermap.find(*i);
  }
  return val;
}

string int_to_string(int i, const string& lettermap)
{
  string val;
  while (i > 0)
  {
    val.push_back(lettermap[i % 10]);
    i /= 10;
  };
  reverse(val.begin(), val.end());
  return val;
}

bool pattern_matches(const string& pattern, const string& s,
                     const string& lettermap)
{
  if (s.size() != pattern.size()) return false;

  string p(pattern);
  auto j = p.begin();
  for (auto i = s.cbegin(); i != s.cend(); ++i, ++j)
  {
    // if we get a character not in the original input, replace all occurrences
    // in the pattern with the substitute character from the string (and the
    // match is implicit on this character)
    if (!isalpha(*j))
    {
      auto c = *j;
      // this letter in the string can't already be accounted for
      if (lettermap.find(*i) != string::npos) return false;
      if (find(s.cbegin(), i, *i) != i) return false;
      replace(j, p.end(), c, *i);
    }
    else if (*i != *j)
    {
      return false;
    }
  }
  return true;
}

void output_solution(const string& w1, const string& w2,
                     const string& pattern, const string& sum,
                     const string& lettermap)
{
  auto i1 = string_to_int(w1, lettermap);
  auto i2 = string_to_int(w2, lettermap);

  cout << w1 << " (" << i1 << ") + "
       << w2 << " (" << i2 << ") = "
       << sum << " (" << pattern << " or " << i1 + i2 << ") where: ";
  string::size_type i = 0;
  for (auto c : lettermap)
  {
    cout << c << "=" << i++;
    if (i != lettermap.size()) cout << ", ";
  }
  cout << ")" << endl;
}

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    cout << "Please supply 2 words and a dictionary file." << endl;
    return 1;
  }

  // Make sure the words are alpha-only and workable
  string word1(argv[1]);
  string word2(argv[2]);
  auto letters = letters_from_words(word1, word2);

  // Get the dictionary
  auto words = get_words(argv[3]);

  // Do the search
  vector<string> solutions;
  do
  {
    // Words can't start with zeroes
    if (word1[0] == letters[0] || word2[0] == letters[0]) continue;

    auto term1 = string_to_int(word1, letters);
    auto term2 = string_to_int(word2, letters);

    auto pattern = int_to_string(term1 + term2, letters);

    auto matcher = [&] (const string& s) {
                     return pattern_matches(pattern, s, letters);
                   };

    auto i = find_if(words.cbegin(), words.cend(), matcher);
    for (; i != words.cend(); i = find_if(++i, words.cend(), matcher))
    {
      if (find(solutions.cbegin(), solutions.cend(), *i) == solutions.cend())
      {
        solutions.push_back(*i);
        output_solution(word1, word2, pattern, *i, letters);
      }
    }

  } while (next_permutation(letters.begin(), letters.end()));

  return 0;
}
