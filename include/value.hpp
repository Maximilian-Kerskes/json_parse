#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <variant>
#include <vector>
class Json {
  public:
    using array = std::vector<Json>;
    using object = std::map<std::string, Json>;
    using storage = std::variant<
		std::nullptr_t,
		bool,
		std::int64_t,
		double,
		std::string,
		array,
		object
	>;

	Json() : value_(nullptr) {}
	Json(std::nullptr_t) : value_(nullptr) {}
	Json(bool value) : value_(value) {}
	Json(std::int64_t value) : value_(value) {}
	Json(int value) : value_(static_cast<std::int64_t>(value)) {}
	Json(double value) : value_(value) {}
	Json(const char* value) : value_(std::string(value)) {}
	Json(std::string value) : value_(std::move(value)) {}
	Json(array value) : value_(std::move(value)) {}
	Json(object value) : value_(std::move(value)) {}

	template <typename T>
	bool holds() const {
        return std::holds_alternative<T>(value_);
	}

	template <typename T>
	T& get() {
		return std::get<T>(value_);
	}

	template <typename T>
	const T& get() const {
		return std::get<T>(value_);
	}

	template <typename T>
	T* get_if() {
		return std::get_if<T>(&value_);
	}

	template <typename T>
	const T* get_if() const {
		return std::get_if<T>(&value_);
	}

	Json& operator[](const std::string& key) {
		if (holds<std::nullptr_t>()) {
			value_ = object{};
		}

		return get<object>()[key];
	}

	const Json& operator[](const std::string& key) const {
		return get<object>().at(key);
	}

	Json& operator[](int index) {
        return get<array>().at(index);
    }

	const Json& operator[](int index) const {
        return get<array>().at(index);
    }

	template <typename Visitor>
	decltype(auto) visit(Visitor&& visitor) {
		return std::visit(std::forward<Visitor>(visitor), value_);
	}

	template <typename Visitor>
	decltype(auto) visit(Visitor&& visitor) const {
		return std::visit(std::forward<Visitor>(visitor), value_);
	}

  private:
    storage value_;

};
