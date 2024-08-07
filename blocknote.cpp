for (const string& word : query.plus_words) {
    if (word_to_document_freqs_.count(word) == 0) {
        continue;
    }
    const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
    for (const auto& [document_id, term_freq] : word_to_document_freqs_.at(word)) {
        const auto& document_data = documents_.at(document_id);
        if (predicate(document_id, document_data.status, document_data.rating)) {
            document_to_relevance[document_id] += term_freq * inverse_document_freq;
        }
    }
}