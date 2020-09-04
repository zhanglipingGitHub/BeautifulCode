//假设传入的容器第 0 号元素不存放内容
template<typename Container>
void InsertSort(Container& container) {

	//依次为第 i 个元素寻找待插入的位置
	for (int i = 2; i < container.size(); ++i) {

		//将第 0 号元素作为哨兵的位置
		container[0] = container[i];

		//前 i - 1 个元素已经有序，寻找第一个比 temp 小的元素
		int j = i - 1;
		for (; container[j] > container[0]; --j) {
			//元素后移
			container[j + 1] = container[j];
		}
		container[j + 1] = container[0];
	}
}