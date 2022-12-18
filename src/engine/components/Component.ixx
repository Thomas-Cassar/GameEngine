export module Component;

export import :BoxCollision;
export import :CameraComponent;
export import :ColorComponent;
export import :InputComponent;
export import :MeshComponent;
export import :MovementComponent;
export import :PlayerComponent;
export import :TextureComponent;
export import :TransformComponent;
import Ecs;

namespace Component
{
export void registerComponents(Ecs::EntityManager& manager)
{
    manager.registerComponent<TransformComponent>();
    manager.registerComponent<MeshComponent>();
    manager.registerComponent<CameraComponent>();
    manager.registerComponent<InputComponent>();
    manager.registerComponent<TextureComponent>();
    manager.registerComponent<MovementComponent>();
    manager.registerComponent<BoxCollision>();
    manager.registerComponent<ColorComponent>();
    manager.registerComponent<PlayerComponent>();
}
} // namespace Component