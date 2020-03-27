#pragma once

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

/*
a template works liks this.
std::string s="times";
auto re=toStdString(1,"day, i say goodbye for",3.0, s, "");
out: 1 day, i say goodbye for 3.0 times;
*/
template <typename String>
std::string  toStdString(const String &args)
{
	return args;
}


//
//template <typename T, typename Target>
//std::string toAppendStrImpl(const T& v, Target result) {
//	toAppend(v, result);
//}
//
//template <typename ... Args>
//std::string toAppendStrImpl(const T& v, Target result) {
//	toAppend(v, result);
//}
//
//template <class T, class... Args>
//typename std::enable_if<sizeof...(Args) >= 2>::type
//	toAppendStrImpl(const T& v, const  Args&... args) {
//	toAppend(v, std::tuple:getLastElement(args...));
//	toAppendStrImpl(vs...);
//}