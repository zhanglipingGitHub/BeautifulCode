//确定中心元素的位置
template<typename Container>
int partition(Container& container, int start, int end) {

	//取第一个元素为 piovt,存入 container[0]
	//此时，piovt 位置就空了，可以存放一个较小的元素
	container[0] = container[start];

	int low = start, high = end;
	int emptyPlace = start;

	//记录当前是查找较大的元素还是查找较小的元素
	bool findSmall = true;

	while (low < high) {

		if (findSmall) {

			//移动 high 位置，直到找到一个较小的元素
			while (high > low && container[high] > container[0]) { high--; }

			container[emptyPlace] = container[high];

			//此时 high 位置为空，可以存放一个较大的元素
			emptyPlace = high;

			findSmall = false;
		}
		else {

			//移送 low 位置，直到找到一个较大的元素
			while (low < high && container[low] <= container[0]) { low++; }

			container[emptyPlace] = container[low];

			//此时 low 位置为空，可以存放一个较大的元素
			emptyPlace = low;

			findSmall = true;
		}
	}

	//将 piovt 元素放入最终的空位置
	container[emptyPlace] = container[0];

	return emptyPlace;
}

//假设传入的容器第 0 号元素不存放内容
template<typename Container>
void quickSortHelper(Container& container, int start, int end) {

	if (start >= end) { return; }

	int piovt = partition(container, start, end);

	//递归调用左右两部分
	quickSortHelper(container, start, piovt - 1);
	quickSortHelper(container, piovt + 1, end);
}

//假设传入的容器第 0 号元素不存放内容
template<typename Container>
void quickSort(Container& container) {

	quickSortHelper(container, 1, container.size() - 1);

}