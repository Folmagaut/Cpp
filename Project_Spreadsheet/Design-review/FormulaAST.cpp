#include "FormulaAST.h"
#include "sheet.h" // Добавляем включение Sheet.h

namespace ASTImpl {

double CellExpr::Evaluate(const SheetInterface& sheet) const {
    const CellInterface* cell = sheet.GetCell(pos_);
    if (!cell) {
        return 0.0; // Пустая ячейка считается 0
    }
    CellInterface::Value value = cell->GetValue();
    if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        try {
            return std::stod(std::get<std::string>(value)); // Пытаемся преобразовать строку в число
        } catch (...) {
            throw FormulaError(FormulaError::Category::Value); // Ошибка, если не удалось преобразовать
        }
    } else {
        throw std::get<FormulaError>(value); // Пробрасываем ошибку из ячейки
    }
}

// ...

} // namespace ASTImpl