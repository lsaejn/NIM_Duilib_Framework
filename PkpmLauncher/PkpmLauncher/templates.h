#pragma once

template<typename T>
class ScopePtrDeletor
{
public:
	ScopePtrDeletor(T* t) :ptr_(t) {}
	~ScopePtrDeletor() { if (ptr) delete ptr; }
	T* ptr_;
};

template<typename func= std::function<void()>>
class ScopeFunc
{
public:
	ScopeFunc(func f)
		:f_(f)
	{
	}
	~ScopeFunc()
	{
		f_();
	}
	func f_;
};

template<typename... Ts>
void UNUSEDPARAMS(const Ts&... /*args*/)
{
}

template<typename R, typename... Args, typename Class>
constexpr int getCountOfPara(R(Class::*)(Args...))
{
	return sizeof...(Args);
}

template <typename F, typename Tuple, std::size_t... Indices>
decltype(auto) forward_to_f(F f, Tuple&& args,
	std::index_sequence<Indices...>) {
	return std::bind(f, std::get<Indices>(std::move(args))...);
}

template <size_t count, typename F, typename ...Args>
std::function<void(void)> makeFunc(F&& f, Args... args)
{
	auto tuple = std::forward_as_tuple(args...);
	auto seq = std::make_index_sequence<count>{};
	return forward_to_f(f, tuple, seq);
}