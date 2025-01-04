#pragma once
#include <functional>
class scope_guard final {
public:
	scope_guard(std::function<void()> func) : function(std::move(func)) {}
	scope_guard(scope_guard&& other) = delete;
	scope_guard(const scope_guard&) = delete;
	void operator=(const scope_guard&) = delete;

	~scope_guard() {
		if (function)
			function();
	}

private:
	std::function<void()> function;
};