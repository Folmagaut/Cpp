#pragma once
#include "date.h"

using namespace std::literals;

class Parser {
public:
	virtual void ParseQuery(std::string_view query) = 0;

	void SetDateFrom(const Date from);
	void SetDateTo(const Date to);
	void SetEarnings(const double earnings);
	void SetTax(const double tax);
	void SetSpents(const double spents);

	const Date GetDateFrom() const;
	const Date GetDateTo() const;
	double GetEarnings() const;
	double GetTax() const;
	double GetSpents() const;

private:
	Date from_ = Date("2000-01-01"s);
	Date to_ = Date("2000-01-01"s);
	double earnings_ = 0.0;
	double tax_ = 13.0;
	double spents_ = 0.0;
};

class Parser_read : public Parser {
public:
	Parser_read(std::string_view query) {
		ParseQuery(query);
	}
	void ParseQuery(std::string_view query) override;
};

class Parser_modify : public Parser {
public:
	Parser_modify(std::string_view query) {
		ParseQuery(query);
	}
	void ParseQuery(std::string_view query) override;
};