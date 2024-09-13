#pragma once
#include <set>
#include <string>
#include <vector>

bool IsValidWord(const std::string& word);

/* Да, в классе SearchServer она тоже присутствует, но конкретно эту проверку мне предложила код-ревьюер при сдаче работы в конце прошлого спринта,
пояснив, что таким образом ошибку при вводе слов можно обнаружить гораздо раньше. У меня возникли проблемы с применением этого метода из класса SearchServer через
двойное двоеточие :: (код упорно не хотел компилироваться и выдавал неясные ошибки), поэтому я просто добавил копию этого метода в виде именованной функции,
чтобы вызывать её из функции SplitIntoWords. В таком варианте благополучно сдал работу. Прошу оставить её здесь, чтобы ошибки ввода выявлялись раньше.
*/ 
std::vector<std::string> SplitIntoWords(const std::string& text);

template <typename StringContainer>
std::set<std::string> MakeUniqueNonEmptyStrings(const StringContainer& strings) {
    std::set<std::string> non_empty_strings;
    for (const std::string& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}