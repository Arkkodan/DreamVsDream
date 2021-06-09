#ifndef DVD_RESOURCE_MANAGER_H
#define DVD_RESOURCE_MANAGER_H

#include <string>
#include <vector>

namespace resource_manager {
	template<typename T>
	void load(const std::string& resource, bool reloadOK = false);

	template<typename T>
	void unload(const std::string& resource);

	template<typename T>
	T* getResource(const std::string& resource);

	template<typename T>
	void loadAll();

	template<typename T>
	std::vector<T*> getAll();
}

#endif // DVD_RESOURCE_MANAGER_H
