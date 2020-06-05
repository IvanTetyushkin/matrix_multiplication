#pragma once
#ifndef HEADER_CPUDIAG
#define HEADER_CPUDIAG

#include <vector>

#include <algorithm>
#include <iostream>
#include <string>

#include <tuple>

#include <numeric>

#include <execution>

#include "common.hpp"

template<typename type, typename allocator>
class base_vector
{
protected:
	std::vector<type, allocator> data;
	virtual std::string get_class() const noexcept
	{
		return "base_vector";
	}
	void print_head() const noexcept
	{
		std::cout << "============================\n";
		std::cout << get_class() << std::endl;
		std::cout << "Vector size:\nstr = " << data.size() << "\n";
	}
	void print_tail() const noexcept
	{
		std::cout << "\n============================\n";
	}
public:
	base_vector(int size) :
		data(size) {}
	void str_dump() const noexcept
	{
		print_head();
		std::cout << "str_dump()\n";
		std::for_each(data.begin(), data.end(), [](const type& value) {
			std::cout << value << "\t";
		});
		print_tail();
	}	
	void col_dump() const noexcept
	{
		print_head();
		std::cout << "col_dump()\n";
		std::for_each(data.begin(), data.end(), [](const type& value) {
			std::cout << value << "\n";
		});
		print_tail();
	}
	const type& operator()(int i) const
	{
		return data[i];
	}
	type& operator()(int i)
	{
		return data[i];
	}
	typename std::vector<type, allocator>::const_iterator begin() const
	{
		return data.begin();
	}
	typename std::vector<type, allocator>::const_iterator end() const
	{
		return data.end();
	}
	typename std::vector<type, allocator>::iterator begin()
	{
		return data.begin();
	}
	typename std::vector<type, allocator>::iterator end()
	{
		return data.end();
	}

	int get_size() const noexcept
	{
		return data.size();
	}
	void fill_with_value(const type& value)
	{
		std::fill(data.begin(), data.end(), value);
	}
	void fill_with_vector(const std::vector<type>& values)
	{
		if (values.size() != get_size())
			throw "bad size base vector";
		std::copy(values.begin(), values.end(), data.begin());
	}
	void change_size(size_t new_size)
	{
		data.resize(new_size);
	}
};

/*
data layout:


*/
constexpr int exists = 1;
constexpr int nonexists = 0;


template<typename type, typename allocator>
class base_diag_matrix
{
private:
	void check_indexes(int i, int j) const
	{
		if (i > str || j > col)
			throw "bad indexes";
		return;
	}
	void check_correctness() const
	{
		if (col < 0 || str < 0)
		{
			throw "negative sizes";
		}

		if (raw_data.size() < str)
		{
			throw "too small";// false wrong
		}


		if (raw_data.size() != ((1+col) *  get_alloc_diag_num() + str))
		{
			throw "size mismatch";// false wrong
		}
		// seems alright...
		return;
	}
	
	


	std::tuple<int, int> get_alloc_diag_between(int current_diag) const noexcept
	{
		int pass_diags = 0;
		int copy_diags = 0;
		for (int i{ 0 }; i < col; i++)
		{
			if (raw_data[i] == exists)
			{
				if (i < current_diag)
				{
					pass_diags++;
				} else if( i > current_diag)
				{ 
					copy_diags++;
				}

			}
		}

		return std::tuple<int, int>(pass_diags, copy_diags);
	}
	
	void  fill_exist_diag_with_vector(int number, const std::vector<type>& new_diag)
	{
		check_correctness();
		if (raw_data[number] != exists)
			throw "try to add  nonexisting diag";
		if (new_diag.size() != col)
			throw "mismatch of adding diag and already existing diag";
		int pass_diags;
		int copy_diags;

		std::tie(pass_diags, copy_diags) = get_alloc_diag_between(number);
		
		std::copy(new_diag.begin(), new_diag.end(), get_diag_info_start() + (col + 1) * pass_diags + 1);
		check_correctness();
	}

	void fill_exist_diag_with_value(int number, type value = 0.0)
	{
		check_correctness();
		if (raw_data[number] != exists)
			throw "try to fill nonexisting diag";

		int pass_diags;
		int copy_diags;

		std::tie(pass_diags, copy_diags) = get_alloc_diag_between(number);
		std::fill_n( get_diag_info_start() + (col  + 1)* pass_diags + 1, col, value);
		check_correctness();
	}

	void alloc_new_diag(int number)
	{
		check_correctness();
		if (raw_data[number] != nonexists)
			throw "try to add already existing diag";

		size_t current_size = raw_data.size();
		raw_data.resize(current_size + 1 +col);
		raw_data[number] = exists;
		int pass_diags;
		int copy_diags;

		std::tie(pass_diags, copy_diags) = get_alloc_diag_between(number);

		std::move_backward( get_diag_info_start() + (col +1) * pass_diags, 
			get_diag_info_start() + (col +1) * (pass_diags + copy_diags), get_diag_info_end());
		raw_data[str + (col + 1) * pass_diags] = number;
		check_correctness();
	}

	void add_new_zero_diag(int number)
	{
		alloc_new_diag(number);

		fill_exist_diag_with_value(number, 0.0);// now works, but problem...
	}

	void print_head() const noexcept
	{
		check_correctness();
		std::cout << "============================\n";
		std::cout << get_class() << std::endl;
		std::cout << "Matrix size:\nstr = "
			<< str <<
			"\ncol = " << col << "\n";
		std::cout << "Alloced size:\n" << raw_data.size() << "\n";

		std::cout << "used diags: " << get_alloc_diag_num() << "\n";
		check_correctness();
	}

	type& get_existing_value(int diag_num, int diag_off) noexcept
	{
		int before, after;
		check_correctness();
		std::tie(before, after) = get_alloc_diag_between(diag_num);

		return *(get_diag_info_start() + before * (1+col) + diag_off + 1);
	}
	const type& get_existing_value(int diag_num, int diag_off) const noexcept
	{
		int before, after;
		std::tie(before, after) = get_alloc_diag_between(diag_num);
		return *(get_diag_info_start() + before * (1+col) +1 + diag_off);

	}


protected:
	std::vector<type, allocator>  raw_data;
	int str;
	int col;

	int get_alloc_diag_num() const noexcept
	{
		int current_diag_num = 0;
		// no for_each_n found in lib
		std::for_each(get_info_header_start(), get_info_header_end(), [&current_diag_num](type status)
		{
			if (status == exists)
				current_diag_num++;
		});
		return current_diag_num;
		// change for size!?
	}
	typename std::vector<type, allocator>::iterator get_diag_info_start() noexcept
	{
		return raw_data.begin() + str;
	}

	typename std::vector<type, allocator>::const_iterator get_diag_info_start() const noexcept
	{
		return raw_data.begin() + str;
	}

	typename std::vector<type, allocator>::iterator get_diag_info_end() noexcept
	{
		return raw_data.end();
	}
	typename std::vector<type, allocator>::const_iterator get_diag_info_end() const noexcept
	{
		return raw_data.end();
	}

	typename std::vector<type, allocator>::iterator get_info_header_start() noexcept
	{
		return raw_data.begin();
	}
	typename std::vector<type, allocator>::const_iterator get_info_header_start() const noexcept
	{
		return raw_data.begin();
	}

	typename std::vector<type, allocator>::iterator get_info_header_end() noexcept
	{
		return raw_data.begin() + str;
	}
	typename std::vector<type, allocator>::const_iterator get_info_header_end() const noexcept
	{
		return raw_data.begin() + str;
	}
	const type* const get_pointer_to_exist_diag(int diag) const noexcept
	{
		if (raw_data[diag] == nonexists)
		{
			throw "error:: try to get pointer to not allocated diag";
		}
		int before, after;
		std::tie(before, after) = get_alloc_diag_between(diag);
		return raw_data.data() + str + (1+col) * before + 1;
	}


	bool check_already_alloced(int diag) const
	{
		return (raw_data[diag] == exists);
	}


	std::tuple<int, int> get_diag_coord(int i, int j) const noexcept
	{
		int offset = j;
		int diag = positive_modulo((i - j), str);
		return std::tuple<int, int>(diag, offset);
	}
public:
	

	base_diag_matrix() = delete;

	base_diag_matrix(int _str, int _col) :
		raw_data(_str),
		str(_str),
		col(_col)
	{
		std::fill_n(get_info_header_start(), _str, static_cast<type>(0.0));
	}
	void fill_diag_with_value(int diag, const type& value)
	{
		check_correctness();
		if (!check_already_alloced(diag))
		{
			alloc_new_diag(diag);
		}
		fill_exist_diag_with_value(diag, value);
	}
	void fill_diag_with_values(int diag, const std::vector<type>& values)
	{
		check_correctness();
		if (values.size() != col)
			throw "bad vector size:: mismatch with matrix size";

		if (!check_already_alloced(diag))
		{
			alloc_new_diag(diag);
		}
		fill_exist_diag_with_vector(diag, values);
	}
	// string_view not compiled... on windows
	virtual std::string get_class() const
	{
		return "base diag matrix";
	}

	virtual void raw_dump() const noexcept final
	{
		print_head();
		std::cout << "raw_dump\n";
		for (auto it = get_info_header_start(); it != get_info_header_end(); it++)
			std::cout << *it << "\t";
		std::cout << "\n";

		for (int i{ 0 }; i < ( get_alloc_diag_num()); i++)
		{
			for (int j{ 0 }; j < col +1; j++)
				std::cout << raw_data[str + i*(col + 1) + j] << "\t";
			std::cout << "\n";
		}

		std::cout << "\n";
		std::cout << "============================\n";
	}

	virtual void pretty_dump() const noexcept final
	{
		print_head();
		std::cout << "pretty_dump\n";
		for (int i{ 0 }; i < str; i++)
		{
			for (int j{ 0 }; j < col; j++)
				std::cout << operator()(i,j) << "\t";
			std::cout << "\n";
		}

		std::cout << "\n";
		std::cout << "============================\n";

	}
	// why not call in std::cout << operator() <<!????
	virtual type operator()(int i, int j) const noexcept final// warn - return copy, not value!!! 
	{
		int diag, offset;
		std::tie(diag, offset) = get_diag_coord(i, j);
		if (raw_data[diag] == nonexists)
			return 0;
		else
			return get_existing_value(diag, offset);
	}

	virtual type& operator()(int i, int j) final// alloc may be...
	{
		int diag, offset;
		std::tie(diag, offset) = get_diag_coord(i, j);
		if (!check_already_alloced(diag))
		{
			add_new_zero_diag(diag);
		}
		return get_existing_value(diag, offset);
	}
};



#endif
