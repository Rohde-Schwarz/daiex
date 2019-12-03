/* @cond HIDDEN_SYMBOLS */

#pragma once

#ifdef _MSC_VER
#pragma warning (push, 0)
#elif __GNUC__
#pragma GCC diagnostic push
#endif

#include <cstddef> // for size_t

namespace memory_mapped_file
{
	unsigned int mmf_granularity();

	class base_mmf
	{
	public:
		explicit base_mmf();
		~base_mmf();
		size_t offset() const { return offset_; }
		size_t mapped_size() const { return mapped_size_; }
		size_t file_size() const { return file_size_; }
		void unmap();
		void close();
		bool is_open() const
		{
			return file_handle_ !=
#if defined(_WIN32)
				(void*)
#endif
				-1;
		}
#if defined(_WIN32)
		typedef void* HANDLE;
#else
		typedef int HANDLE;
#endif
		HANDLE file_handle() const
		{
			return file_handle_;
		}
	protected:
		size_t query_file_size_();
		char* data_;
		size_t offset_;
		size_t mapped_size_;
		size_t file_size_;
		int granularity_;
		HANDLE file_handle_;
#if defined(_WIN32)
		HANDLE file_mapping_handle_;
#endif
	};

	class read_only_mmf: public base_mmf
	{
	public:
#if defined(_WIN32)
		explicit read_only_mmf(wchar_t const* pathname = 0, bool map_all = true);
    void open(wchar_t const* pathname, bool map_all = true);
#else
    explicit read_only_mmf(char const* pathname = 0, bool map_all = true);
    void open(char const* pathname, bool map_all = true);
#endif
		char const* data() const { return data_; }
		void map(size_t offset = 0, size_t size = 0);
	};

	enum mmf_exists_mode
	{
		if_exists_fail,
		if_exists_just_open,
		if_exists_map_all,
		if_exists_truncate,
	};

	enum mmf_doesnt_exist_mode
	{
		if_doesnt_exist_fail,
		if_doesnt_exist_create,
	};

	class writable_mmf: public base_mmf
	{
	public:
#if defined(_WIN32)
		explicit writable_mmf(wchar_t const* pathname = 0,
			mmf_exists_mode exists_mode = if_exists_fail,
			mmf_doesnt_exist_mode doesnt_exist_mode = if_doesnt_exist_create);
		void open(wchar_t const* pathname,
			mmf_exists_mode exists_mode = if_exists_fail,
			mmf_doesnt_exist_mode doesnt_exist_mode = if_doesnt_exist_create);
#else
    explicit writable_mmf(char const* pathname = 0,
      mmf_exists_mode exists_mode = if_exists_fail,
      mmf_doesnt_exist_mode doesnt_exist_mode = if_doesnt_exist_create);
    void open(char const* pathname,
      mmf_exists_mode exists_mode = if_exists_fail,
      mmf_doesnt_exist_mode doesnt_exist_mode = if_doesnt_exist_create);
#endif
		char* data() { return data_; }
		void map(size_t offset = 0, size_t size = 0);
		bool flush();
	};
}

/*** @endcond ***/