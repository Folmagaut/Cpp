#include "formula.h"
#include "FormulaAST.h"
//#include "sheet.h" // Добавляем включение sheet.h
#include "common.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << fe.ToString();//"#ARITHM!";
}

namespace {
class Formula : public FormulaInterface {
public:
// Реализуйте следующие методы:
    explicit Formula(std::string expression) try : ast_(ParseFormulaAST(std::move(expression))) {
    } catch (const FormulaException& fexc) {
        throw;
    }

    Value Evaluate(const SheetInterface& sheet) const override {
        try {
            return ast_.Execute(sheet);
        } catch (const FormulaError& ferr) {
            return ferr;
        }
    }

    std::string GetExpression() const override {
        std::ostringstream out;
        ast_.PrintFormula(out);
        return std::move(out).str();
    }

    std::vector<Position> GetReferencedCells() const override { // Добавляем реализацию
        /* std::vector<Position> result(ast_.GetCells().begin(), ast_.GetCells().end());
        std::sort(result.begin(), result.end());
        result.erase(std::unique(result.begin(), result.end()), result.end());
        return result; */
        std::vector<Position> cells;
        for (auto cell : ast_.GetCells()) {
            if (cell.IsValid()) cells.push_back(cell);
        }
        cells.resize(std::unique(cells.begin(), cells.end()) - cells.begin());
        return cells;
    }

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try {
        return std::make_unique<Formula>(std::move(expression));
    }
    catch (...) {
        throw FormulaException("");
    }
}