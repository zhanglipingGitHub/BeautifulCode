template<typename Container>
void mergeSort(Container& container) {

	//归并时临时存储容器
	Container tempContainer(container.size());

	for (int m = 1; m < container.size(); m *= 2) {

		//[i, i+m), [i+m, i+2*m)两组归并
		for (int i = 1; i < container.size();) {

			int leftEdge = i + m, rightEdge = leftEdge + m;

			//防止越界
			leftEdge = leftEdge < container.size() ? leftEdge : container.size();
			rightEdge = rightEdge < container.size() ? rightEdge : container.size();

			int low = i, high = i + m;
			int j = i;

			while (low < leftEdge && high < rightEdge) {

				if (container[low] < container[high]) {
					tempContainer[j++] = container[low];
					low++;
				}
				else {
					tempContainer[j++] = container[high];
					high++;
				}
			}

			while (low < leftEdge) {
				tempContainer[j++] = container[low];
				low++;
			}

			while (high < rightEdge) {
				tempContainer[j++] = container[high];
				high++;
			}

			//将排序后的子序列复制回原数组
			for (int k = i; k < rightEdge; k++) {
				container[k] = tempContainer[k];
			}

			i = rightEdge;
		}
	}
}