
#pragma once
#include <new>
#include <iostream>
using namespace std;

/**********************************************************************/
//һ���ռ���������ʵ��
#define THROW_BAD_ALLOC cerr<<"out of memory"<<endl;exit(1)

template <int inst>
class Alloc_malloc
{
public:
	//Allocate��������ռ�
	static void* Allocate(size_t n)
	{
		void* result = malloc(n);
		//����ʧ�ܣ�ʹ��oom_malloc()���³�������
		if (result == NULL)
			return Oom_malloc(n);
		return NULL;
	}

	//Deallocate�����ͷſռ�
	static void Deallocate(void* p, size_t /* size*/)
	{
		free(p);
	}

	//Reallocate���ڸ�����Ҫ�����Ѿ����ڵĿռ�Ĵ�С
	static void* Reallocate(void* p, size_t size)
	{
		void* result = realloc(p, size);
		//����ʧ�ܣ�ʹ��oom_realloc()��������
		if (result == NULL)		
			return Oom_realloc(p, size);
		return NULL;
	}

	//Set_malloc_handler���������������û��ṩ���ͷſռ�ĺ���ָ��
	static void(*Set_malloc_handler(void(*f)())) ()
	{
		void(*old)() = _Malloc_alloc_oom_handler;
		_Malloc_alloc_oom_handler = f;
		return (old);
	}

private:
	//ͨ���û��ṩ���ͷſռ䣨�ͷ��Լ��Ѿ������˵Ŀռ䣩�ĺ������ϵ��ͷſռ䲢���
	//ֱ���ͷų��Ŀռ��㹻���������Ŀռ�
	//����û�û���ṩ�ͷſռ�ĺ����������쳣
	static void* Oom_malloc(size_t size)
	{
		void* result;
		void(*My_malloc_handler)();
		for (;;)
		{
			My_malloc_handler = _Malloc_alloc_oom_handler;
			if (0 == My_malloc_handler)//�û�û���ṩ�ͷſռ�ĺ���
				THROW_BAD_ALLOC;
			(*My_malloc_handler)();
			result = malloc(size);
			if (result)
				return result;
		}
	}

	static void* Oom_realloc(void* p, size_t size)
	{
		void* result;
		void(*My_malloc_handler) ();
		for (;;)
		{
			My_malloc_handler = _Malloc_alloc_oom_handler;
			if (0 == my_malloc_handler)
				THROW_BAD_ALLOC;
			(*My_malloc_handler)();
			result = realloc(p, size);
			if (result)
				return result;
		}
	}

private:
	static void(*_Malloc_alloc_oom_handler)();
};

//�����ʼ����̬��Ա���� _malloc_alloc_oom_handler
template <int inst>
void(*Alloc_malloc<inst>::_Malloc_alloc_oom_handler)() = 0;

/***********************�����ռ���������ʵ��******************************/
enum{ ALINE = 8 };//����Ļ��ֱ߽磬���Ϊ4�ռ������ʲ��ߣ�����ʱ����Ҫһ���ָ���ȥ����
enum{MAX_BYTES=128};//С�ڴ����Ľ���
enum{ NFREELISTS = MAX_BYTES / ALINE };//free_list�ĸ���

template <int inst>
class default_Alloc
{
public:
	static void* Allocate(size_t size)//���ٿռ�
	{
		obj* volatile result;
		//��ռ䣬ʹ��һ���ռ������������ڴ�
		printf("����%d�ֽڵĿռ�\n", size);
		if (size > MAX_BYTES)
		{
			printf("����ռ����128�ֽڣ�ʹ��һ���ռ�������\n");
			return Alloc_malloc<inst>::Allocate(size);
		}


		//С�ռ䣬ʹ�ö����ռ������������ڴ�
		//��ȥ��Ӧ��������������
		int index = Free_list_index(size);
		result = _free_list[index];
		if (NULL == result)//����������û�пռ䣬���ڴ��������ռ�
		{
			printf("��%d������������û�пռ䣬���ڴ��������ռ�\n", index);
			result = (obj*)Refill(size);
			return result;
		}
		_free_list[index] = result->free_list_link;
		printf("�ڵ�%d�����������л�ȡ��%d���ֽڵĿռ�\n", index, size);
		return result;
	}

	static void Deallocate(void* p, size_t n)//�ͷſռ�
	{
		if (n > 128)
		{
			printf("�ͷ�%d���ֽڵĿռ䣬�黹��ϵͳ\n", n);
			Alloc_malloc<inst>::Deallocate(p, n);
			return;
		}
		obj* pCur = (obj*)p;
		int index = Free_list_index(n);
		pCur->free_list_link = _free_list[index];
		_free_list[index] = pCur;
		printf("�ͷ�%d���ֽڵĿռ䣬�ҽӵ�%d������������\n\n", n, index);
	}

private:
	static size_t Round_up(size_t bytes)//�����ϵ���ALINE��������
	{
		return (((bytes)+ALINE - 1)&~(ALINE - 1));
	}

	static size_t Free_list_index(size_t bytes)//���ڼ�����free_lists�е��±�λ��
	{
		return (bytes - 1) / ALINE;
		//return (bytes+(ALINE-1))/ALINE-1;  //SGI�еĴ���ʽ
	}
	static void* Refill(size_t bytes) //���ڴ��������ռ�
	{
		int nobjs = 20;
		//����chunk_alloc���������Ի�ȡnobjs���ڴ��
		char* chunk = Chunk_alloc(bytes, nobjs);
		obj* result;
		if (1 == nobjs)//ֻ���뵽һ��ռ�Ĵ�С�����ÿ�ռ䷵�ظ��û�ʹ��
		{
			printf("���ڴ����ɹ����뵽%d��%d�ֽڴ�С���ڴ��\n", nobjs, bytes);
			return chunk;
		}

		//��������ж�飬������һ�鷵�ظ��ã������Ĺҽӵ�����������
		printf("���ڴ����ɹ����뵽%d��%d�ֽڴ�С���ڴ��\n", nobjs, bytes);
		result = (obj*)chunk;
		int n = Free_list_index(bytes);
		obj* next = (obj*)(chunk + bytes);
		while (1)
		{
			next->free_list_link = _free_list[n];
			_free_list[n];
			next = (obj*)((char*)next + bytes);
			if ((char*)next == chunk + nobjs*bytes)
			{
				printf("�ɹ���ʣ�µ�%d��%d�ֽڴ�С���ڴ��ҽӵ�%d������������\n", nobjs - 1, bytes, n);
				break;
			}

		}
		return result;
	}


private:
	static char* Chunk_alloc(size_t size, int& nobjs)
	{
		char* result;
		size_t total_bytes = size*nobjs;
		size_t bytes_left = _end - _start;
		if (bytes_left >= total_bytes)//�ڴ���ﻹ���㹻�Ŀռ�
		{
			printf("���ڴ������%d��%d�ֽڴ�С���ڴ��\n", nobjs, size);
			result = _start;
			_start += total_bytes;
			return result;
		}
		else if (bytes_left >= size)//�ڴ����߻����ṩ����һ���ڴ��
		{
			nobjs = bytes_left / size;
			result = _start;
			_start += nobjs*size;
			printf("���ڴ������%d��%d�ֽڴ�С���ڴ��\n", nobjs, size);
			return result;
		}
		else
		{
			//�ڴ�������һ���ڴ��ռ�Ĵ�С��û�У���ϵͳ�������ռ�����ڴ��
			if (bytes_left > 0)//����ڴ���ﻹʣ�пռ�
			{
				int n = Free_list_index(bytes_left);
				((obj*)_start)->free_list_link = _free_list[n];
				_free_list[n] = (obj*)_start;
			}
			size_t bytetoget = 2 * total_bytes + Round_up(_heap_size >> 4);
			_start = (char*)malloc(bytetoget);
			printf("�ڴ�ؿռ䲻��%d�ֽڣ���ϵͳ����%d�ֽڴ�С���ڴ��\n", size, bytetoget);
			if (NULL == _start)//ϵͳ���û���㹻�Ŀռ�
			{
				printf("ϵͳ�ռ䲻�㣬���ܷ���%d�ֽڴ�С���ڴ��\n", bytetoget);
				//�ӵ�ǰ��������ʼ���������������������
				for (size_t i = size; i <= MAX_BYTES; i++)
				{
					int n = Free_list_index(i);
					if (_free_list[n])//�����������������пռ�
					{
						_start = (char*)_free_list[n];
						_end = _start + i;
						printf("�ڵ�%d�����������л�ȡ��%d���ֽڵĿռ�����ڴ����\n", n, size);
						//�ݹ�����Լ����������nobjs
						return (Chunk_alloc(size, nobjs));
					}
				}
				//�Ѿ���ɽ��ˮ���ˣ����Ե���һ���ռ�������������
				printf("malloc�Ѿ�ʧ�ܣ���������������Ҳû�б������С���ڴ��ˣ�ʹ��һ���ռ����������ٴγ�������ռ�\n");
				_end = 0;
				_start = (char*)Alloc_malloc<inst>::Allocate(size);
			}
			_end = _start + bytetoget;
			_heap_size += bytetoget;
			//�ݹ�����Լ����������nobjs
			return (Chunk_alloc(size, nobjs));
		}
	}

private:
	union obj
	{
		union obj* free_list_link;
		char client_data[1];
	};

private:
	static char* _start;
	static char* _end;
	static size_t _heap_size;

	static obj* volatile _free_list[NFREELISTS];
};
//�����ʼ����ľ�̬��Ա����
template <int inst>
char* default_Alloc<inst> ::_start = 0;

template <int inst>
char* default_Alloc<inst> ::_end = 0;

template <int inst>
size_t default_Alloc<inst> ::_heap_size = 0;

template <int inst>
typename default_Alloc<inst>::obj* volatile default_Alloc<inst>::_free_list[NFREELISTS] =
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
