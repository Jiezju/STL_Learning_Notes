#pragma once
#include <iostream>

using namespace std;
/*
 * jjhou �γ̵����һ�ڴ���
 * 
 */
class MyString
{
public:
	static  size_t DCtor;
	static  size_t Ctor;
	static  size_t CCtor;
	static  size_t CAsgn;
	static  size_t MCtor;
	static  size_t MAsgn;
	static  size_t Dtor;

private:
	char* _data;
	size_t _len;

	void _init_data(const char* s)
	{
		_data = new char[_len + 1];
		memcpy(_data, s, _len); // s -> _data
		_data[_len] = '\0';
	}

public:
	// default ctor
	MyString() : _data(NULL), _len(0) { ++DCtor; }

	// ctor
	MyString(const char* p): _len(strlen(p))
	{
		++Ctor;
		_init_data(p);
	}

	// copy ctor
	MyString(const MyString& str): _len(str._len)
	{
		++CCtor;
		_init_data(str._data); // copy
	}

	// move ctor
	MyString(MyString&& str) noexcept: _data(str._data), _len(str._len)
	{
		++MCtor;
		str._len = 0;
		str._data = NULL; // ����ָ�룬���ͷ��ڴ棬�ڴ滹����
	}

	// copy assignment
	MyString& operator=(const MyString& str)
	{
		++CAsgn;
		if (this != &str)
		{
			if (_data)
				delete _data;
			_len = str._len;
			_init_data(str._data);
		}

		return *this;
	}

	// move assigment
	MyString& operator=(MyString&& str) noexcept
	{
		++MAsgn;
		if (this != &str)
		{
			if (_data)
				delete _data;

			_data = str._data; // move
			_len = str._len;
			str._len = 0;
			str._data = NULL;
		}

		return *this;
	}

	// dtor
	virtual ~MyString()
	{
		++Dtor;
		// �����ƶ�ָ�룬����������֮ǰ��_data ָ����ڴ��Ѿ��ͷ�
		if (_data)
			delete _data;
	}

	bool operator<(const MyString& rhs) const
	{
		// ת���� string �����бȽ�
		return std::string(this->_data) < std::string(rhs._data);
	}

	bool operator=(const MyString& rhs) const
	{
		// ת���� string �����бȽ�
		return std::string(this->_data) == std::string(rhs._data);
	}

	char* get() const { return _data; }
};

// ��̬���� ������Ϊ ��Ĺ�����ı�
size_t MyString::DCtor = 0;
size_t MyString::Ctor = 0;
size_t MyString::CCtor = 0;
size_t MyString::CAsgn = 0;
size_t MyString::MCtor = 0;
size_t MyString::MAsgn = 0;
size_t MyString::Dtor = 0;

// �Զ���� hash ����ʹ�� set
namespace std
{
	// �ػ� stl hash �ķ���������һ���º���������һ������ adapter
	template<>
	struct hash<MyString>
	{
		size_t operator()(const MyString& s)
		{
			// ת�������е� string �ػ� hash
			return hash<string>()(string(s.get()));
		}
	};
};
