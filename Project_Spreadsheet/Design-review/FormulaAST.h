#pragma once

#include "FormulaLexer.h"
#include "common.h"
#include "sheet.h" // Добавляем включение sheet.h

#include <forward_list>
#include <functional>
#include <stdexcept>

namespace ASTImpl {

class Expr {
public:
    virtual ~Expr() = default;
    virtual double Evaluate(const SheetInterface& sheet) const = 0; // Добавляем параметр SheetInterface
    virtual void Print(std::ostream& out) const = 0;
};

class CellExpr : public Expr {
public:
    CellExpr(Position pos) : pos_(pos) {}
    double Evaluate(const SheetInterface& sheet) const override; // Реализация вычисления
    void Print(std::ostream& out) const override;

private:
    Position pos_;
};

// Другие классы Expr...

} // namespace ASTImpl

class ParsingError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class FormulaAST {
public:
    explicit FormulaAST(std::unique_ptr<ASTImpl::Expr> root_expr);
    FormulaAST(FormulaAST&&) = default;
    FormulaAST& operator=(FormulaAST&&) = default;
    ~FormulaAST();

    double Execute(const SheetInterface& sheet) const; // Изменяем Execute
    void Print(std::ostream& out) const;
    void PrintFormula(std::ostream& out) const;

private:
    std::unique_ptr<ASTImpl::Expr> root_expr_;
    std::forward_list<Position> cells_; // Добавляем список ячеек
    std::forward_list<Position> GetCells() const; // Метод для получения списка ячеек
};

FormulaAST ParseFormulaAST(std::istream& in);
FormulaAST ParseFormulaAST(const std::string& in_str);