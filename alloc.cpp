
//�����ʦ���ڴ����γ̵� std::alloc Դ��ķ���


enum { __ALIGN = 8 };                        // block �ļ��
enum { __MAX_BYTES = 128 };                  // block ����� bytes
enum { __NFREELISTS = __MAX_BYTES / __ALIGN }; //free-lists �ĸ���


//ģ���࣬��һ���������߳��йأ����ﲻ������
//�ڶ��������������Ҳû�й�ϵ
template<bool thread, int inst> 
class __default_alloc_template
{
private:
	static size_t ROUND_UP(size_t bytes);

	union obj
	{
		union *free_list_link;

	};//embaded pointer������Ҳ������ strut

	//free_list ���飬ÿһ��λ���Ϲ��ز�ͬ���� block ������
	static obj* volatile free_list[__NFREELISTS];

	//�����ֽ��������� free_list �����еı�š�
	//����˵���� bytes = 20,���Ӧ�� free_list �����е� #2 λ�ã�����2��
	static size_t FREELIST_INDEX(size_t bytes);

	//�����ϵͳ��ȡ�µ��ڴ��
	static void* refill(size_t n);

	static char* chunk_alloc(size_t size, int& nobjs);

	//ָ�� memory pool �Ŀ�ʼλ��
	static char* start_free;

	//ָ�� memory pool �Ľ���λ��
	static char* end_free;

	//�ܹ�������ڴ����ֽ���
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

//������Ѿ��Ƕ���֮����ֽڴ�С
template<bool thread, int inst>
void * __default_alloc_template<thread, inst>::refill(size_t n)
{
	//��ʾ�����������ӵ������
	int nobjs = 20;

	//��ȡһ���ڴ� chunk,ע�� nobj �� pass by reference
	char *chunk = chunk_alloc(n, nobjs);

	//���ص��ڴ��ֻ��һ�� block ��С�������ֱ����Ϊ�������
	//����Ҫ���к��������Ӷ���
	if (nobjs == 1)
		return chunk;

	obj **my_free_list = free_list + FREELIST_INDEX(n);

	//����õ��ڴ���ͷָ��ֱ����Ϊ�������
	obj	*result = (obj*)chunk;

	//������������� free_list ��
	*my_free_list = (obj*)(chunk + n);

	obj *next_obj = *my_free_list;
	obj* current_obj;

	//����һ���ڴ�����Ϊ block ����
	for (int i = 1; ; ++i)
	{
		current_obj = next_obj;

		//�������һ��
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
	//����Ҫ���ڴ���С
	size_t total_bytes = size * nobjs;

	//memory pool ʣ����ڴ���С
	size_t bytes_left = end_free - start_free;

	char* result;

	//memory pool �㹻���� nobjs �� size ��С�� block
	if (bytes_left >= total_bytes)
	{
		result = start_free;
		start_free += total_bytes;
		return result;
	}

	//���Է�����һ�� block
	else if (bytes_left >= size)
	{
		nobjs = bytes_left / size;
		total_bytes = size * nobjs;
		result = start_free;
		start_free += total_bytes;
		return result;
	}

	//һ�����ֲ���
	else
	{
		//�µģ���Ҫ�����ϵͳ��ȡ���ڴ���С
		size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);

		//��������ڴ���Ƭ���Ը��ڴ���Ƭ��СΪ һ�� block ��С�����뵽��Ӧ������ͷ�ڵ�λ��
		if (bytes_left > 0)
		{
			//�ҵ��ڴ���Ƭ��С��Ӧ�� free_list �е�λ��
			obj **my_free_list = free_list + FREELIST_INDEX(bytes_left);

			//���뵽��Ӧ�����ͷ�������л�������
			((obj*)start_free)->free_list_link = *my_free_list;
			*my_free_list = (obj*)start_free;
		}

		//�����ϵͳ��ȡ�µ��ڴ��
		start_free = (char*)malloc(bytes_to_get);

		//���˵����ʱ����ϵͳ�ڴ�ľ����޷��ɹ����� bytes_to_get ��С���ڴ棬
		//��� size ��ģ��������������ȡһ�� block �������и�
		if (0 == start_free)
		{
			obj *p;
			for (i = size; i <= __MAX_BYTES; i += __ALIGN)
			{
				obj **my_free_list = free_list + FREELIST_INDEX(i);

				p = *my_free_list;

				if (p != nullptr)
				{
					//�൱������һ�� block ��ȥ��
					*my_free_list = p->free_list_link;

					//���г�������һ����Ϊ memory pool
					start_free = (char*)p;
					end_free = start_free + i;

					//��һ�ε��� chunk_alloc�����ʱ��һ�����Ե��óɹ����ֳ�һ�� size ��С�� block
					return(chunk_alloc(size, nobjs));
				}
			}
			
			//�Ҳ������Ҳû�п��� block ���ã���ʱ���õ�һ���ڴ���䷽ʽ
			end_free = 0;
			start_free = (char*)malloc_allocate(bytes_to_get);
		}

		heap_size += bytes_to_get;
		end_free = start_free + bytes_to_get;

		//�ݹ����һ�Σ� �����һ�� if �ж�������
		return(chunk_alloc(size, nobjs));
	}
}

template<bool thread, int inst>
void * __default_alloc_template<thread, inst>::allocate(size_t n)
{
	//���û���Ҫ��õ��ڴ���С�������� block ��С���� 128 ʱ��
	//�����ڴ����ʽ�ͻ�ʧЧ��ת����������һ���ӿڣ��ýӿڿ�Ϊ�û��ṩ
	//���Ƶ� new_handler ���ơ����ﲻ����������
	if (n > (size_t)__MAX_BYTES)
	{
		return malloc_allocate(n);
	}

	// ��� n ��Ӧ�� block ���������ͷָ���ָ��
	obj **my_free_list = free_list + FREELIST_INDEX(n);

	//ͷָ��
	obj *result = *my_free_list;

	//��������û�пյ� block ����ʹ�ã���Ҫ���·����ڴ�
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
	//�������һ���ڴ沢�����ɵ�ǰ���ڴ������Ʒ����ȥ��
	if (n > (size_t)__MAX_BYTES)
	{
		malloc_deallocate(p, n);
		return;
	}

	// ��� n ��Ӧ�� block ���������ͷָ���ָ��
	obj **my_free_list = free_list + FREELIST_INDEX(n);
	obj *q = (obj*)p;

	//���뵽���������ͷ�ڵ�λ��
	q->free_list_link = *my_free_list;
	*my_free_list = q;
}
