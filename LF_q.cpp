#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

vector<int> ReadLineRatings() {
    vector<int> document_ratings;
    int n;
    cin >> n;
    for (int i = 0; i < n; ++i) {
        int m;
        cin >> m;
        document_ratings.push_back(m); 
    }
    return document_ratings;
}

/*    int ComputeAverageRating(const vector<int>& ratings) {
        int rate = 0;
        int rate_size = ratings.size();
        for (int i = 0; i < rate_size; ++i) {
            rate += ratings[i];
        }
        if (rate_size != 0) {
            rate = rate / rate_size;
        }
        return rate;
    }
*/
int main() {
    
   vector<int> vec = ReadLineRatings();

   for (int i : vec) {
    cout << i << " "s;
   }
   cout << endl;

}