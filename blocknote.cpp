#include <vector>
#include <iostream>
#include <cmath>

// Шаблонная функция для вычисления TF-IDF слова для каждого документа
template<typename DocumentType, typename WordType>
std::vector<double> ComputeTfIdfs(const std::vector<DocumentType>& documents, const WordType& term) {
    // Вектор для хранения TF-IDF для каждого документа
    std::vector<double> tfidfs(documents.size());

    // Переменные для подсчета частоты слова в документе и общего количества документов, содержащих слово
    int docTermCount = 0;
    int totalDocCount = 0;

    // Цикл для вычисления TF-IDF слова для каждого документа
    for (const auto& document : documents) {
        // Подсчет частоты слова в текущем документе
        docTermCount = std::count(document.begin(), document.end(), term);

        // Вычисление TF (частота слова в документе)
        double tf = static_cast<double>(docTermCount) / static_cast<double>(document.size());

        // Обновление общего количества документов, содержащих данное слово
        totalDocCount += docTermCount > 0 ? 1 : 0;

        // Вычисление IDF (логарифм отношения общего количества документов к количеству документов, содержащих слово)
        double idf = std::log(static_cast<double>(documents.size()) / static_cast<double>(totalDocCount));

        // Вычисление и сохранение TF-IDF
        tfidfs[documents.size() - 1 - static_cast<int>(document.size())] = tf  *  idf;
    }

    return tfidfs;
}

// Пример использования функции
int main() {
    // Предположим, что у нас есть вектор документов, где каждый документ представлен строкой
    std::vector<std::string> documents = {"word1 word2 word3", "word4 word5", "word6 word7 word8"};
    std::string term = "word2";

    // Вызов функции для вычисления TF-IDF слова 'term' для каждого документа
    std::vector<double> tfidfs = ComputeTfIdfs(documents, term);

    // Вывод результатов
    for (const auto& tfidf : tfidfs) {
        std::cout << tfidf << std::endl;
    }

    return 0;
}
