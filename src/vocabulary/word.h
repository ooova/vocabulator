#ifndef VOCABULARY_WORD_H
#define VOCABULARY_WORD_H

#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace vocabulary {

using Translation =
    std::pair<std::vector<std::string> /*foreign*/, std::vector<std::string> /*examples*/
              >;

using Word = std::pair<std::string, Translation>;

class Vocabulary {
public:
    Vocabulary();

    void addWord(std::string_view const word, Translation&& translation);

private:
    std::map<std::string /*native*/, Translation> vocabulary_{};
};

}  // namespace vocabulary

#endif  // VOCABULARY_WORD_H
