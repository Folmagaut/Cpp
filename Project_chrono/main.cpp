#include "log_duration.h"

#include <iostream>

using namespace std;

class StreamUntier {
public:
    explicit StreamUntier(istream& stream) : tied_before_(stream.tie(nullptr)), input_(stream) {
    }
    
    ~StreamUntier() {
        input_.tie(tied_before_);
    }

private:
    ostream* tied_before_;
    istream& input_;
};

int main() {
    LOG_DURATION("\\n with tie"s);

    StreamUntier guard(cin);
    int i;
    while (cin >> i) {
        cout << i * i << "\n"s;
    }

    return 0;
}