//从[rootIndex, lastIndex)之间的数据待调整
template<typename Container>
void heapAdjust(Container& container, int rootIndex, int lastIndex)
{
	if (rootIndex >= lastIndex) { return; }

	typedef typename iterator_traits<Container::iterator>::value_type value_type;

	//暂存根节点的值
	value_type rootVal = container[rootIndex];

	//寻找根节点应该插入的位置
	//当根节点 index=i 时，左子节点 index=2*i，右子节点 index=2*i+1
	for (int i = rootIndex << 1; i < lastIndex; i = i << 1) {
		
		//获得左右子节点较小的
		if (i + 1 < lastIndex && container[i + 1] < container[i]) { i++; }

		if (i < lastIndex && container[i] < rootVal) {
			container[rootIndex] = container[i];
			rootIndex = i;
		}
		else
		{
			break;
		}
	}

	//插入根节点的值
	container[rootIndex] = rootVal;
}

//假设传入的容器第 0 号元素不存放内容
template<typename Container>
void createMinHeap(Container& container) {

	for (int i = (container.size() - 1) >> 1; i > 0; i--) {

		heapAdjust(container, i, container.size());
	}
}

//实际上是用小根堆，从大到小排序了
template<typename Container>
void heapSort(Container& container) {

	createMinHeap(container);

	typedef typename iterator_traits<Container::iterator>::value_type value_type;


	for (int i = container.size() - 1; i > 1; --i) {

		//把开始最小的值放到最后
		//这样实际就是按照从大到小排序了
		value_type temp = container[i];
		container[i] = container[1];
		container[1] = temp;

		heapAdjust(container, 1, i);
	}
}