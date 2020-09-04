//对容器进行以增量 delta 的插入排序
//假设传入的容器第 0 号元素不存放内容
template<typename Container>
void ShellInsert(Container& container, int delta) {

	//delta 次插入排序
	for (int i = 1; i < 1 + delta; ++i) {

		//插入排序
		for (int j = i + delta; j < container.size(); j += delta) {

			container[0] = container[j];

			int k = j - delta;

			for (; k > 0 && container[k] > container[0]; k -= delta) {
				container[k + delta] = container[k];
			}

			container[k + delta] = container[0];
		}
	}
}

//假设传入的容器第 0 号元素不存放内容
//delta 定义增量序列
template<typename Container>
void ShellSort(Container& container, const vector<int>& delta) {
	
	//以增量序列 delta 依次进行希尔插入
	for (int e : delta) {
		ShellInsert(container, e);
	}
}