//假设传入的容器第 0 号元素不存放内容
template<typename Container>
void selectSort(Container& container) {

	typedef typename iterator_traits<Container::iterator>::value_type value_type;

	for (int i = 1; i < container.size() - 1; ++i) {
		
		int minIndex = i;

		//在剩下的元素中寻找最小的
		for (int j = i + 1; j < container.size(); ++j) {

			if (container[j] < container[minIndex]) {
				minIndex = j;
			}
		}

		if (minIndex != i) {
			value_type temp = container[minIndex];
			container[minIndex] = container[i];
			container[i] = temp;
		}
	}
}