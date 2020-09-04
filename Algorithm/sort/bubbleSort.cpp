//假设传入的容器第 0 号元素不存放内容
template<typename Container>
void bubbleSort(Container& container) {

	bool hasExchanged = true;

	//总共需要比较 n - 1 趟
	for (int i = 1; i < container.size() && hasExchanged; ++i) {

		hasExchanged = false;

		//第 i 趟需要比较 n - i 次
		for (int j = 0; j < container.size() - i; ++j) {

			if (container[j] > container[j + 1]) {

				hasExchanged = true;

				container[0] = container[j];
				container[j] = container[j + 1];
				container[j + 1] = container[0];
			}
		}
	}
}