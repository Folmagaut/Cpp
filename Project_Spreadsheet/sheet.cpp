#include "sheet.h"

#include "common.h"
#include "formula.h"
#include "cell.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::ResizeCells(Position pos) {
    if (cells_.size() <= static_cast<size_t>(pos.row)) {
        cells_.resize(static_cast<size_t>(pos.row) + 1);
    }
    if (cells_[pos.row].size() <= static_cast<size_t>(pos.col)) {
        cells_[pos.row].resize(static_cast<size_t>(pos.col) + 1);
    }
}

void Sheet::SetCell(Position pos, std::string text) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }

    ResizeCells(pos);

    // Сохраняем текущее состояние ячейки
    std::unique_ptr<Cell> old_cell;
    if (cells_[pos.row][pos.col]) {
        old_cell = std::move(cells_[pos.row][pos.col]);
    }

    // Создаем новую ячейку
    cells_[pos.row][pos.col] = std::make_unique<Cell>(*this);
    cells_[pos.row][pos.col]->Set(text);

    // Получаем новые зависимости
    std::vector<Position> new_dependencies = cells_[pos.row][pos.col]->GetReferencedCells();

    if (!new_dependencies.empty()) {
        for (const auto cell_pos : new_dependencies) {
            if (cell_pos == pos) {
                throw CircularDependencyException("Circular dependency detected");
            }
            //std::cout << "Debug: Setting cell at " << cell_pos.ToString() << std::endl;
            ResizeCells(cell_pos);
            const CellInterface* cell = GetCell(cell_pos);
            if (cell == nullptr) {
                //SetCell(cell_pos, "");
                cells_[cell_pos.row][cell_pos.col] = std::make_unique<Cell>(*this);
                UpdateDependencies(cell_pos, {});
            }
        }
    }
    // Обновляем зависимости
    UpdateDependencies(pos, new_dependencies);

    // Проверяем наличие циклических зависимостей
    if (HasCircularDependencies(pos)) {
        // Если есть циклические зависимости, восстанавливаем старое состояние ячейки
        UpdateDependencies(pos, {});
        if (old_cell) {
            cells_[pos.row][pos.col] = std::move(old_cell);
        } else {
            cells_[pos.row][pos.col].reset();
        }
        throw CircularDependencyException("Circular dependency detected");
    }
    
    // Обновляем размер области печати
    UpdatePrintableSize(pos);
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }
    if (static_cast<size_t>(pos.row) < cells_.size() && static_cast<size_t>(pos.col) < cells_[pos.row].size() && cells_[pos.row][pos.col]) {
        return cells_[pos.row][pos.col].get();
    }
    return nullptr;
}
CellInterface* Sheet::GetCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }
    if (static_cast<size_t>(pos.row) < cells_.size() && static_cast<size_t>(pos.col) < cells_[pos.row].size() && cells_[pos.row][pos.col]) {
        return cells_[pos.row][pos.col].get();
    }
    return nullptr;
}

void Sheet::ClearCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }
    if (static_cast<size_t>(pos.row) < cells_.size() && static_cast<size_t>(pos.col) < cells_[pos.row].size()) {
        cells_[pos.row][pos.col].reset(); //->Clear();
        UpdateDependencies(pos, {}); // Обновляем зависимости
        RecalculatePrintableSize(); // Обновляем размер
    }
}

Size Sheet::GetPrintableSize() const {
    return printable_size_;
}

void Sheet::PrintValues(std::ostream& output) const {
    for (int row = 0; row < printable_size_.rows; ++row) {
        for (int col = 0; col < printable_size_.cols; ++col) {
            Position pos = {row, col};
            const CellInterface* cell = GetCell(pos);
            if (cell) {
                std::visit([&output](const auto& value) {
                    output << value;
                }, cell->GetValue());
            }
            if (col < printable_size_.cols - 1) {
                output << '\t';
            }
        }
        output << '\n';
    }
}
void Sheet::PrintTexts(std::ostream& output) const {
    for (int row = 0; row < printable_size_.rows; ++row) {
        for (int col = 0; col < printable_size_.cols; ++col) {
            Position pos = {row, col};
            const CellInterface* cell = GetCell(pos);
            if (cell) {
                output << cell->GetText();
            }
            if (col < printable_size_.cols - 1) {
                output << '\t';
            }
        }
        output << '\n';
    }
}

void Sheet::UpdatePrintableSize(Position pos) {
    printable_size_.rows = std::max(printable_size_.rows, pos.row + 1);
    printable_size_.cols = std::max(printable_size_.cols, pos.col + 1);
}

void Sheet::RecalculatePrintableSize() {
    printable_size_ = {0, 0};
    for (size_t row = 0; row < cells_.size(); ++row) {
        for (size_t col = 0; col < cells_[row].size(); ++col) {
            if (cells_[row][col]) {
                printable_size_.rows = std::max(printable_size_.rows, static_cast<int>(row) + 1);
                printable_size_.cols = std::max(printable_size_.cols, static_cast<int>(col) + 1);
            }
        }
    }
}

/*
Проверка наличия зависимостей: Мы проверяем, есть ли уже зависимости для ячейки pos в карте dependencies_.
Удаление старых зависимостей: Если зависимости существуют, мы удаляем их.
Итерация по зависимостям: Мы перебираем все ячейки, от которых зависела pos.
Получение множества зависимостей: Мы получаем ссылку на множество зависимостей для каждой зависимой ячейки.
Удаление pos: Мы удаляем pos из множества зависимостей зависимой ячейки.
Проверка пустоты: Мы проверяем, остались ли у зависимой ячейки другие зависимости.
Удаление ячейки: Если у зависимой ячейки больше нет зависимостей, мы удаляем её из карты.
Удаление pos: Мы удаляем pos из карты зависимостей.
Проверка новых зависимостей: Мы проверяем, есть ли новые зависимости для добавления.
Преобразование в множество: Мы преобразуем вектор новых зависимостей в множество для удаления дубликатов.
Добавление зависимостей: Мы добавляем новые зависимости для pos в карту.
Итерация по новым зависимостям: Мы перебираем все новые зависимости.
Добавление pos: Мы добавляем pos в множество зависимостей для каждой новой зависимости.
Инвалидация кэша: Мы вызываем InvalidateCache, чтобы сбросить кэш для всех ячеек, зависящих от pos.
Этот метод корректно обновляет карту зависимостей, удаляя старые зависимости и добавляя новые.
Он также инвалидирует кэш, чтобы гарантировать, что все зависимые ячейки будут пересчитаны.
*/

void Sheet::UpdateDependencies(Position pos, const std::vector<Position>& new_dependencies) {
    // 1. Проверяем, есть ли уже зависимости для данной позиции (pos) в карте dependencies_
    if (dependencies_.count(pos)) {
        // 2. Если зависимости для pos существуют, начинаем их удаление

        // 3. Итерируемся по всем зависимостям, которые pos имеет в данный момент
        for (const auto& dep : dependencies_[pos]) {
            // 4. Получаем ссылку на множество зависимостей для каждой зависимой ячейки (dep)
            auto& deps = dependencies_[dep];

            // 5. Удаляем pos из множества зависимостей dep
            deps.erase(pos);

            // 6. Проверяем, осталось ли у dep какие-либо зависимости
            if (deps.empty()) {
                // 7. Если у dep больше нет зависимостей, удаляем dep из карты dependencies_
                dependencies_.erase(dep);
            }
        }
        // 8. После удаления всех зависимостей, удаляем pos из карты dependencies_
        dependencies_.erase(pos);
    }

    // 9. Проверяем, есть ли новые зависимости (new_dependencies) для добавления
    if (!new_dependencies.empty()) {
        // 10. Преобразуем вектор новых зависимостей в unordered_set для удаления дубликатов
        std::unordered_set<Position, PositionHash> unique_dependencies(new_dependencies.begin(), new_dependencies.end());

        // 11. Добавляем новые зависимости для pos в карту dependencies_
        dependencies_[pos] = unique_dependencies;

        // 12. Итерируемся по каждой новой зависимости
        /* for (const auto& dep : unique_dependencies) {
            // 13. Добавляем pos в множество зависимостей для каждой новой зависимости
            dependencies_[dep].insert(pos);
        } */
    }
    // 14. Вызываем метод InvalidateCache, чтобы сбросить кэш для всех ячеек, зависящих от pos
    InvalidateCache(pos);
}

void Sheet::InvalidateCache(Position pos) {
    // 1. Создаем unordered_set invalidated для хранения позиций ячеек, кэш которых уже был инвалидирован.
    // Это нужно, чтобы избежать повторной обработки одной и той же ячейки.
    std::unordered_set<Position, PositionHash> invalidated;

    // 2. Создаем очередь queue для обхода графа зависимостей в ширину (BFS).
    std::queue<Position> queue;

    // 3. Добавляем начальную позицию pos в очередь.
    queue.push(pos);

    // 4. Добавляем начальную позицию pos в множество invalidated, чтобы отметить, что её кэш уже инвалидирован
    invalidated.insert(pos);

    // 5. Начинаем цикл, пока очередь не пуста.
    while (!queue.empty()) {
        // 6. Получаем позицию ячейки из начала очереди.
        Position cell_pos = queue.front();

        // 7. Удаляем позицию ячейки из начала очереди.
        queue.pop();

        // 8. Получаем указатель на ячейку по позиции cell_pos.
        Cell* cell = dynamic_cast<Cell*>(GetCell(cell_pos));

        // 9. Проверяем, существует ли ячейка (cell != nullptr).
        if (cell) {
            // 10. Если ячейка существует, вызываем её публичный метод InvalidateCache(), чтобы инвалидировать её кэш.
            cell->InvalidateCache();
        }

        // 11. Итерируемся по всем зависимостям в карте dependencies_.
        // dependencies_ - это граф зависимостей, где ключ - это позиция ячейки, а значение - множество позиций ячеек, от которых она зависит.
        for (const auto& [dep_pos, deps] : dependencies_) {
            // 12. Проверяем, зависит ли ячейка dep_pos от текущей ячейки cell_pos (deps.count(cell_pos))
            // и не была ли ячейка dep_pos уже инвалидирована (!invalidated.count(dep_pos)).
            if (deps.count(cell_pos) && !invalidated.count(dep_pos)) {
                // 13. Если ячейка dep_pos зависит от cell_pos и её кэш еще не был инвалидирован,
                // добавляем её в очередь для дальнейшей обработки.
                queue.push(dep_pos);

                // 14. Добавляем ячейку dep_pos в множество invalidated, чтобы отметить, что её кэш был инвалидирован.
                invalidated.insert(dep_pos);
            }
        }
    }
}

/*
visited: Множество для отслеживания посещенных ячеек во время обхода графа.
recursionStack: Множество для отслеживания ячеек в текущем стеке рекурсии.
Если ячейка находится в стеке рекурсии и мы снова ее встречаем, это означает, что есть цикл.
dfs (Depth-First Search): Лямбда-функция для обхода графа зависимостей в глубину.
Добавление в visited: Добавляем текущую ячейку в множество посещенных ячеек.
Добавление в recursionStack: Добавляем текущую ячейку в стек рекурсии.
Проверка зависимостей: Проверяем, есть ли зависимости для текущей ячейки в карте dependencies_.
Итерация по зависимостям: Если зависимости существуют, перебираем их.
Проверка посещения: Проверяем, была ли зависимая ячейка уже посещена.
Рекурсивный вызов dfs: Если зависимая ячейка не была посещена, рекурсивно вызываем dfs для нее.
Обнаружение цикла: Если рекурсивный вызов dfs вернул true, значит, найден цикл, возвращаем true.
Обнаружение цикла в стеке: Если зависимая ячейка находится в стеке рекурсии, значит, найден цикл, возвращаем true.
Удаление из recursionStack: Удаляем текущую ячейку из стека рекурсии, так как мы закончили ее обработку.
Нет цикла: Если циклы не найдены, возвращаем false.
Вызов dfs: Вызываем dfs для начальной позиции pos и возвращаем результат.
Этот метод использует алгоритм DFS для обнаружения циклов в графе зависимостей, начиная с заданной позиции pos.
Он корректно отслеживает посещенные ячейки и ячейки в стеке рекурсии, чтобы обнаружить циклы.
*/

bool Sheet::HasCircularDependencies(Position pos/* , const std::unordered_set<Position, PositionHash>& dependencies */) {
    /* std::vector<Position> referenced_cells = cells_[pos.row][pos.col]->GetReferencedCells();
    //auto referenced_cells = GetReferencedCells();
    for (Position ref : referenced_cells) {
        if (ref == pos) {
            return true;
        }
    } */
    // 1. Создаем множество visited для отслеживания посещенных ячеек
    std::unordered_set<Position, PositionHash> visited;
    // 2. Создаем множество recursionStack для отслеживания ячеек в текущем стеке рекурсии
    std::unordered_set<Position, PositionHash> recursionStack;

    // 3. Определяем лямбда-функцию dfs (Depth-First Search) для обхода графа зависимостей
    std::function<bool(Position)> dfs = [&](Position current_pos) {
        // 4. Добавляем текущую ячейку в множество посещенных ячеек
        visited.insert(current_pos);
        // 5. Добавляем текущую ячейку в стек рекурсии
        recursionStack.insert(current_pos);

        // 6. Проверяем, есть ли зависимости для текущей ячейки в карте dependencies_
        if (dependencies_.count(current_pos)) {
            // 7. Если зависимости существуют, итерируемся по ним
            for (const auto& dep : dependencies_[current_pos]) {
                // 8. Проверяем, была ли зависимая ячейка уже посещена
                if (!visited.count(dep)) {
                    // 9. Если зависимая ячейка не была посещена, рекурсивно вызываем dfs для нее
                    if (dfs(dep)) {
                        // 10. Если рекурсивный вызов dfs вернул true, значит, найден цикл, возвращаем true
                        return true;
                    }
                } else if (recursionStack.count(dep)) {
                    // 11. Если зависимая ячейка находится в стеке рекурсии, значит, найден цикл, возвращаем true
                    return true;
                }
            }
        }

        // 12. Удаляем текущую ячейку из стека рекурсии, так как мы закончили ее обработку
        recursionStack.erase(current_pos);
        // 13. Если циклы не найдены, возвращаем false
        return false;
    };

    // 14. Вызываем dfs для начальной позиции pos и возвращаем результат
    return dfs(pos);
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}

/* 
bool Cell::HasCyclicDependency(const Position& my_pos) const {
    auto referenced_cells = GetReferencedCells();
    for (Position ref : referenced_cells) {
        if (ref == my_pos) {
            return true;
        }
    }
}

bool Cell::HasPathTo(const Position& from, const Position& to,
    std::unordered_set<Position, PositionHasher>& visited) const {
    if (from == to) {
        return true;
    }

}
*/