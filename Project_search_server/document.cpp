#include "document.h"

using namespace std;

Document::Document() = default;
Document::Document(int id, double relevance, int rating)
    : id(id)
    , relevance(relevance)
    , rating(rating) {
    }
int id = 0;
double relevance = 0.0;
int rating = 0;


ostream& operator<<(ostream& output, const Document& document) {
    return output << "{ document_id = "s << document.id << ", relevance = "s << document.relevance << ", rating = "s << document.rating << " }"s;
}