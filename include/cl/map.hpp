#pragma once

#include <map>
#include <string>

#include "exception.hpp"

namespace cl
{
template <typename T>
class map
{
public:
	typedef std::map<std::string,T> map_type;
	typedef std::pair<std::string,T> pair_type;
	
private:
	map_type _map;
	
public:
	map() {}
	~map() {}
	
	class iterator
	{
	private:
		typename map_type::iterator iter;
		
	public:
		iterator(const typename map_type::iterator &i)
		  : iter(i)
		{
			
		}
		
		bool operator != (const iterator &i) const
		{
			return iter != i.iter;
		}
		
		bool operator == (const iterator &i) const
		{
			return iter == i.iter;
		}
		
		iterator &operator ++ ()
		{
			++iter;
			return *this;
		}
		
		T &operator *()
		{
			return iter->second;
		}
		
		T &operator -> ()
		{
			return iter->second;
		}
		
		const T &operator *() const
		{
			return iter->second;
		}
		
		const T &operator -> () const
		{
			return iter->second;
		}
	};
	
	class const_iterator
	{
	private:
		typename map_type::const_iterator iter;
		
	public:
		const_iterator(const typename map_type::const_iterator &i)
		  : iter(i)
		{
			
		}
		
		bool operator != (const const_iterator &i) const
		{
			return iter != i.iter;
		}
		
		bool operator == (const const_iterator &i) const
		{
			return iter == i.iter;
		}
		
		const_iterator &operator ++ ()
		{
			++iter;
			return *this;
		}
		
		const T &operator *() const
		{
			return iter->second;
		}
		
		const T &operator -> () const
		{
			return iter->second;
		}
	};
	
	iterator begin()
	{
		return iterator(_map.begin());
	}
	
	const_iterator begin() const
	{
		return const_iterator(_map.cbegin());
	}
	
	const_iterator cbegin() const
	{
		return begin();
	}
	
	iterator end()
	{
		return iterator(_map.end());
	}
	
	const_iterator end() const
	{
		return const_iterator(_map.cend());
	}
	
	const_iterator cend() const
	{
		return end();
	}
	
	void insert(const std::string &key, T elem)
	{
		_map.insert(pair_type(key,elem));
	}
	
	void insert(const pair_type &pair)
	{
		_map.insert(pair);
	}
	
	iterator find(const std::string &key)
	{
		return iterator(_map.find(key));
	}
	
	const_iterator find(const std::string &key) const
	{
		return const_iterator(_map.find(key));
	}
	
	void remove(const std::string &key)
	{
		_map.erase(_map.find(key));
	}
	
	T &operator [](const std::string &key) throw(cl::exception)
	{
		auto iter = find(key);
		if(iter == end())
		{
			throw cl::exception((std::string("there is no object '") + key + std::string("' in map")).data());
		}
		return *iter;
	}
	
	const T &operator [](const std::string &key) const throw(cl::exception)
	{
		auto iter = find(key);
		if(iter == end())
		{
			throw cl::exception(std::string("there is no object '") + key + std::string("' in map"));
		}
		return *iter;
	}
};
}
