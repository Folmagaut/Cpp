#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

class Tower {
public:

    Tower(int disks_num) {
        FillTower(disks_num);
    }

    int GetDisksNum() const {
        return disks_.size();
    }

    void SetDisks(int disks_num) {
        FillTower(disks_num);
    }

    void AddToTop(int disk) {
        int top_disk_num = disks_.size()  - 1;
        if (disks_.size() != 0 && disk >= disks_[top_disk_num]) {
            throw invalid_argument("Невозможно поместить большой диск на маленький");
        } else {
            disks_.push_back(disk);
        }
    }

    // disks_num - количество перемещаемых дисков
    // destination - конечная башня для перемещения
    // buffer - башня, которую нужно использовать в качестве буфера для дисков
 void MoveDisks(int disks_num, Tower& destination, Tower& buffer) {
      if (disks_num == 1) {
        int top_disk = disks_.back();
        disks_.pop_back();
        destination.AddToTop(top_disk);
    } else {
        MoveDisks(disks_num - 1, buffer, destination);
        destination.AddToTop(disks_.back());
        disks_.pop_back();
        buffer.MoveDisks(disks_num - 1, destination, *this);
    }
 }
 void ShowDisks(int tower) {
    cout << "Size of tower["s << tower << "] = "s << disks_.size() << endl;
    for (size_t i = 0; i < disks_.size(); ++i) {
        cout << disks_[i];
    }
    cout << endl;
 }

private:
    vector<int> disks_;

    void FillTower(int disks_num) {
        for (int i = disks_num; i > 0; i--) {
            disks_.push_back(i);
        }
    }
};

void SolveHanoi(vector<Tower>& towers) {
    int disks_num = towers[0].GetDisksNum();
    towers[0].MoveDisks(disks_num, towers[2], towers[1]);
}

int main() {
    int towers_num = 3;
    int disks_num = 3;
    vector<Tower> towers;
    // Добавим в вектор три пустые башни.
    for (int i = 0; i < towers_num; ++i) {
        towers.push_back(0);
    }
    // Добавим на первую башню три кольца.
    towers[0].SetDisks(disks_num);

    /* towers[0].ShowDisks(0);
    towers[1].ShowDisks(1);
    towers[2].ShowDisks(2); */
    SolveHanoi(towers);
    /* towers[0].ShowDisks(0);
    towers[1].ShowDisks(1);
    towers[2].ShowDisks(2); */

}