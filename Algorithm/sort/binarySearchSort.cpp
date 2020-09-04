//假设传入的容器第 0 号元素不存放内容
template<typename Container>
void BinarySearchSort(Container& container) {

	//依次为第 i 个元素寻找待插入的位置
	for (int i = 2; i < container.size(); ++i) {

		//将第 0 号元素作为哨兵的位置
		container[0] = container[i];

		int left = 1, right = i - 1;

		//二分查找
		while (left <= right) {

			int mid = (left + right) / 2;

			if (container[mid] > container[0]) {

				right = mid - 1;
			}
			else {

				left = mid + 1;
			}
		}

		//将 [left, i - 1] 之间的元素依次后移，空出 left 位置
		for (int j = i - 1; j >= left; --j) {

			container[j + 1] = container[j];
		}

		container[left] = container[0];
	}
}