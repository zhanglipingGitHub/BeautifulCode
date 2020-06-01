
//侯捷老师的内存管理课程的 std::alloc 源码的分析


enum { __ALIGN = 8 };                        // block 的间隔
enum { __MAX_BYTES = 128 };                  // block 的最大 bytes
enum { __NFREELISTS = __MAX_BYTES / __ALIGN }; //free-lists 的个数


//模板类，第一个参数与线程有关，这里不做讨论
//第二个参数与分配器也没有关系
template<bool thread, int inst> 
class __default_alloc_template
{
private:
	static size_t ROUND_UP(size_t bytes);

	union obj
	{
		union *free_list_link;

	};//embaded pointer，这里也可以用 strut

	//free_list 数组，每一个位置上挂载不同长度 block 的链表
	static obj* volatile free_list[__NFREELISTS];

	//根据字节数，求在 free_list 数组中的编号。
	//比如说传入 bytes = 20,则对应于 free_list 数组中的 #2 位置，返回2。
	static size_t FREELIST_INDEX(size_t bytes);

	//向操作系统索取新的内存块
	static void* refill(size_t n);

	static char* chunk_alloc(size_t size, int& nobjs);

	//指向 memory pool 的开始位置
	static char* start_free;

	//指向 memory pool 的结束位置
	static char* end_free;

	//总共分配的内存块的字节数
	static size_t heap_size;

public:

	static void* allocate(size_t n);

	static void deallocate(void *p, size_t n);
};

typedef __default_alloc_template<false, 0> alloc;

template<bool thread, int inst>
size_t __default_alloc_template<thread, inst>::ROUND_UP(size_t bytes)
{
	return (bytes + __ALIGN - 1) & ~(__ALIGN - 1);
}

//传入的已经是对齐之后的字节大小
template<bool thread, int inst>
void * __default_alloc_template<thread, inst>::refill(size_t n)
{
	//表示链表所能链接的最长长度
	int nobjs = 20;

	//获取一块内存 chunk,注意 nobj 是 pass by reference
	char *chunk = chunk_alloc(n, nobjs);

	//返回的内存块只有一个 block 大小，则可以直接作为结果返回
	//不需要进行后续的链接动作
	if (nobjs == 1)
		return chunk;

	obj **my_free_list = free_list + FREELIST_INDEX(n);

	//将获得的内存块的头指针直接作为结果返回
	obj	*result = (obj*)chunk;

	//把链表挂在数组 free_list 上
	*my_free_list = (obj*)(chunk + n);

	obj *next_obj = *my_free_list;
	obj* current_obj;

	//将这一块内存链接为 block 链表
	for (int i = 1; ; ++i)
	{
		current_obj = next_obj;

		//链表最后一个
		if (i == nobjs - 1)
		{
			current_obj->free_list_link = nullptr;
			break;
		}
		else
		{
			next_obj = (obj*)((char*)current_obj + n);
			current_obj->free_list_link = next_obj;
		}
	}

	return result;
}

template<bool thread, int inst>
char * __default_alloc_template<thread, inst>::chunk_alloc(size_t size, int & nobjs)
{
	//所需要的内存块大小
	size_t total_bytes = size * nobjs;

	//memory pool 剩余的内存块大小
	size_t bytes_left = end_free - start_free;

	char* result;

	//memory pool 足够分配 nobjs 个 size 大小的 block
	if (bytes_left >= total_bytes)
	{
		result = start_free;
		start_free += total_bytes;
		return result;
	}

	//可以分至少一个 block
	else if (bytes_left >= size)
	{
		nobjs = bytes_left / size;
		total_bytes = size * nobjs;
		result = start_free;
		start_free += total_bytes;
		return result;
	}

	//一个都分不到
	else
	{
		//新的，需要向操作系统索取的内存块大小
		size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);

		//如果存在内存碎片，以该内存碎片大小为 一个 block 大小，插入到对应的链表头节点位置
		if (bytes_left > 0)
		{
			//找到内存碎片大小对应的 free_list 中的位置
			obj **my_free_list = free_list + FREELIST_INDEX(bytes_left);

			//插入到对应链表的头部，进行回收利用
			((obj*)start_free)->free_list_link = *my_free_list;
			*my_free_list = (obj*)start_free;
		}

		//向操作系统索取新的内存块
		start_free = (char*)malloc(bytes_to_get);

		//如果说，此时操作系统内存耗尽，无法成功分配 bytes_to_get 大小的内存，
		//向比 size 大的，最近的链表上索取一个 block ，进行切割
		if (0 == start_free)
		{
			obj *p;
			for (i = size; i <= __MAX_BYTES; i += __ALIGN)
			{
				obj **my_free_list = free_list + FREELIST_INDEX(i);

				p = *my_free_list;

				if (p != nullptr)
				{
					//相当于切了一块 block 出去了
					*my_free_list = p->free_list_link;

					//把切出来的这一块作为 memory pool
					start_free = (char*)p;
					end_free = start_free + i;

					//再一次调用 chunk_alloc，这个时候，一定可以调用成功，分出一个 size 大小的 block
					return(chunk_alloc(size, nobjs));
				}
			}
			
			//右侧的链表也没有空闲 block 可用，此时调用第一种内存分配方式
			end_free = 0;
			start_free = (char*)malloc_allocate(bytes_to_get);
		}

		heap_size += bytes_to_get;
		end_free = start_free + bytes_to_get;

		//递归调用一次， 进入第一个 if 判断条件内
		return(chunk_alloc(size, nobjs));
	}
}

template<bool thread, int inst>
void * __default_alloc_template<thread, inst>::allocate(size_t n)
{
	//当用户需要获得的内存块大小大于最大的 block 大小，即 128 时，
	//这种内存管理方式就会失效，转而调用另外一个接口，该接口可为用户提供
	//类似的 new_handler 机制。这里不做过多讨论
	if (n > (size_t)__MAX_BYTES)
	{
		return malloc_allocate(n);
	}

	// 获得 n 对应的 block 所在链表的头指针的指针
	obj **my_free_list = free_list + FREELIST_INDEX(n);

	//头指针
	obj *result = *my_free_list;

	//所在链表没有空的 block 可以使用，需要重新分配内存
	if (result == nullptr)
	{
		return refill(ROUND_UP(n));
	}

	*my_free_list = result->free_list_link;

	return result;
}

template<bool thread, int inst>
void __default_alloc_template<thread, inst>::deallocate(void * p, size_t n)
{
	//传入的这一块内存并不是由当前的内存分配机制分配出去的
	if (n > (size_t)__MAX_BYTES)
	{
		malloc_deallocate(p, n);
		return;
	}

	// 获得 n 对应的 block 所在链表的头指针的指针
	obj **my_free_list = free_list + FREELIST_INDEX(n);
	obj *q = (obj*)p;

	//插入到所在链表的头节点位置
	q->free_list_link = *my_free_list;
	*my_free_list = q;
}
