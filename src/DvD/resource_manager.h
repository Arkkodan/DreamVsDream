#ifndef DVD_RESOURCE_MANAGER_H
#define DVD_RESOURCE_MANAGER_H

#include <string>
#include <vector>

namespace resource_manager {
	/// @brief Load a resource
	/// @typeparam T Any supported resource type (see implementation file)
	/// @param resource Name of a resource
	/// @param reloadOK Reload the resource if true. Attempt to find resource if false.
	/// @returns Whether the resource was successfully loaded
	template<typename T>
	bool load(const std::string& resource, bool reloadOK = false);

	/// @brief Unload a resource
	/// @typeparam T Any supported resource type (see implementation file)
	/// @param resource Name of a resource
	template<typename T>
	void unload(const std::string& resource);

	/// @brief Get a pointer to a resource
	/// @details This resource is owned by the resource manager.
	/// Do not call free or delete on this resource.
	/// @details Attempts to load the resource if not yet loaded.
	/// @typeparam T Any supported resource type (see implementation file)
	/// @param resource Name of a resource
	/// @returns A pointer to the resource if successful, nullptr if not
	template<typename T>
	T* getResource(const std::string& resource);

	/// @brief An overrideable method to load resources from a manifest file
	/// @typeparam T Any supported resource type (see implementation file)
	template<typename T>
	void loadFromManifest();

	/// @brief Get all resources from a manifest file
	/// @details Does not transfer ownership.
	/// @details Call loadFromManifest<T> before calling this.
	/// @typeparam T Any supported resource type (see implementation file)
	/// @returns A vector of pointers to resources
	template<typename T>
	std::vector<T*> getFromManifest();
}

#endif // DVD_RESOURCE_MANAGER_H
