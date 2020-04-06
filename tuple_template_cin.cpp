//--------function--------
//print tuple elements like this:
//[7.5,zhang,2]

//--------critial points-------
//1. variadic template
//2. how to handle first and last element differently
//3. class(structrue) template

template<int IDX, int MAX, typename... Args>
struct PRINT_TUPLE
{
	static void print(std::ostream& os, const std::tuple<Args...>& t)
	{
		os << std::get<IDX>(t) << (IDX == MAX - 1 ? "" : ",");
		PRINT_TUPLE<IDX + 1, MAX, Args...>::print(os, t);
	}
};

template<int MAX, typename... Args>
struct PRINT_TUPLE<MAX, MAX, Args...>
{
	static void print(std::ostream& os, const std::tuple<Args...>& t)
	{

	}
};

template<typename... Args>
std::ostream& operator<<(std::ostream& os, const std::tuple<Args...>& t)
{
	os << "[";
	PRINT_TUPLE<0, sizeof...(Args), Args...>::print(os, t);
	os << "]";
	return os;
}

int main()
{
	
	std::cout << std::make_tuple(7.5, std::string("zhang"), 2) << std::endl;

	system("pause");
}