module;
#include <exception>
#include <filesystem>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
export module Graphics:ResourceManager;
export import :Resource;
import :Texture2D;
import :Model;

export namespace Graphics
{
template <typename T,
          typename =
              std::enable_if<std::is_trivially_constructible<T, std::filesystem::path const&, ResourceManager&>::value>>
class TResourceManager {
public:
    T& getResource(ResourceId id)
    {
        auto resource{resourceStore.find(id)};
        if (resource == std::end(resourceStore))
        {
            using namespace std::string_literals;
            throw std::exception(("Could not find a resource with id "s + std::to_string(id)).c_str());
        }
        return resource->second;
    }

    ResourceId getResourceID(std::filesystem::path filePath, ResourceManager& resourceManager)
    {
        if (!std::filesystem::is_regular_file(filePath))
        {
            using namespace std::string_literals;
            throw std::exception(("Could not find a file with the name "s + filePath.string()).c_str());
        }
        filePath = std::filesystem::canonical(filePath);

        // If this resource is already loaded return existing id
        auto idIterator{pathToId.find(filePath)};
        if (idIterator != std::end(pathToId))
        {
            return idIterator->second;
        }

        // We need to load the resource
        ResourceId const resourceID{resourceIdGenerator.getNewId()};
        pathToId.emplace(filePath, resourceID);
        T temp{filePath, resourceManager};
        resourceStore.emplace(resourceID, std::move(temp));
        return resourceID;
    }

protected:
    Core::IdGenerator resourceIdGenerator;
    std::unordered_map<ResourceId, T> resourceStore;
    std::unordered_map<std::filesystem::path, ResourceId> pathToId;
};

export class ResourceManager {
public:
    template <typename T>
    ResourceId getResourceId(std::filesystem::path const& path);

    template <typename T>
    T& getResource(ResourceId id);

private:
    TResourceManager<Model> models;
    TResourceManager<Texture2D> textures;
};
}; // namespace Graphics