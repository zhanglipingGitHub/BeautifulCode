//-------功能--------
//禁止复制基类，任何类只要继承了这个基类，则无法实现复制动作，包括拷贝构造和拷贝赋值

//------- 注意点 -------
//1. 它的析构函数不一定需要声明为 virtual，因为它并不含有数据成员，不会造成子类析构时的内存泄露问题

class Uncopyable
{
protected:
	Uncopyable() {}
	~Uncopyable() {}

private:

	//1. 显示声明拷贝构造和拷贝赋值函数，则编译器不会为我们自动生成
	//2. 将其声明为私有类型，外部无法访问，实现无法复制的目的
	//3. 只有声明而没有实现，这样当有 member function 或者 friend function 调用时，会出现链接错误
	Uncopyable(const Uncopyable&);
	Uncopyable& operator=(const Uncopyable&);
};

//当尝试为 NotCopy 类型进行复制操作时，在调用其 base class 复制函数时会被拒绝，从而实现禁止复制
//可以 private 继承
class NotCopy : private Uncopyable
{

};