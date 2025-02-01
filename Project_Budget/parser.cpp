#include <stdexcept>
#include <iostream>
#include <string>

#include "date.h"
#include "parser.h"
// разработайте менеджер личного бюджета самостоятельно

void Parser::SetDateFrom(const Date from) {
	from_ = from;
}
void Parser::SetDateTo(const Date to) {
	to_ = to;
}
void Parser::SetEarnings(const double earnings) {
	earnings_ = earnings;
}
void Parser::SetTax(const double tax) {
	tax_ = tax;
}
void Parser::SetSpents(const double spents) {
	spents_ = spents;
}
const Date Parser::GetDateFrom() const {
	return from_;
}
const Date Parser::GetDateTo() const {
	return to_;
}
double Parser::GetEarnings() const {
	return earnings_;
}
double Parser::GetTax() const {
	return tax_;
}
double Parser::GetSpents() const {
	return spents_;
}

void Parser_read::ParseQuery(std::string_view query) {
	std::string_view query_type = query.substr(0, query.find_first_of(' '));
	query.remove_prefix(query_type.size() + 1);
	std::string_view date_from_str = query.substr(0, query.find_first_of(' '));
    std::string_view date_to_str = query.substr(query.find_first_of(' ') + 1, query.find_last_of(' ') - query.find_first_of(' ') - 1);
	try {
        SetDateFrom(Date(date_from_str));
        SetDateTo(Date(date_to_str));
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid input format: " << e.what() << std::endl;
    } catch (const std::out_of_range& e) {
        std::cerr << "Earnings value out of range: " << e.what() << std::endl;
    } catch (const std::exception& e) { 
        std::cerr << "An error occurred during parsing: " << e.what() << std::endl;
    }
}

void Parser_modify::ParseQuery(std::string_view query) {
	std::string_view query_type = query.substr(0, query.find_first_of(' '));
	query.remove_prefix(query_type.size() + 1);
	std::string_view date_from_str = query.substr(0, query.find_first_of(' '));
    std::string_view date_to_str = query.substr(query.find_first_of(' ') + 1, query.find_last_of(' ') - query.find_first_of(' ') - 1);
    std::string_view earnings_or_spents_or_tax_str = query.substr(query.find_last_of(' ') + 1);

    try {
        SetDateFrom(Date(date_from_str));
        SetDateTo(Date(date_to_str));
		if (query_type == "Earn"sv) {
			SetEarnings(std::stod(std::string(earnings_or_spents_or_tax_str))); // Convert string_view to string
		}
		if (query_type == "PayTax"sv) {
			SetTax(std::stod(std::string(earnings_or_spents_or_tax_str)));
		}
		if (query_type == "Spend"sv) {
			SetSpents(std::stod(std::string(earnings_or_spents_or_tax_str)));
		}
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid input format: " << e.what() << std::endl;
        // Handle error appropriately (e.g., throw an exception, set default values)
    } catch (const std::out_of_range& e) {
        std::cerr << "Earnings_or_spents_or_tax_str value out of range: " << e.what() << std::endl;
        // Handle error
    } catch (const std::exception& e) { // Catch other potential exceptions
        std::cerr << "An error occurred during parsing: " << e.what() << std::endl;
        // Handle error
    }
}