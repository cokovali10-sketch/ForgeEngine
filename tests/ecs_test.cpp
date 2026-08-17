#include <cassert>
#include <iostream>

#include "forge/ecs/components.h"
#include "forge/ecs/world.h"

int main() {
    forge::ecs::World world;

    auto e1 = world.create();
    auto e2 = world.create();
    auto e3 = world.create();

    assert(world.alive(e1));
    assert(world.alive(e2));
    assert(world.alive(e3));

    world.add<forge::Velocity>(e1, forge::Vec2{10.0f, 20.0f});
    world.add<forge::Velocity>(e2, forge::Vec2{-5.0f, 1.0f});

    auto* v = world.get<forge::Velocity>(e1);
    assert(v && v->value.x == 10.0f && v->value.y == 20.0f);

    world.destroy(e2);
    assert(!world.alive(e2));
    assert(world.alive(e1));

    // Component removed with entity.
    assert(world.get<forge::Velocity>(e2) == nullptr);

    // Recreated entity gets a new version.
    auto e2b = world.create();
    assert(e2b.index == e2.index);
    assert(e2b.version > e2.version);
    assert(world.alive(e2b));

    // each() visits remaining component holders.
    int count = 0;
    world.each<forge::Velocity>([&](forge::ecs::Entity, forge::Velocity&) { count++; });
    assert(count == 1);

    world.destroy(e3);
    assert(!world.alive(e3));

    std::cout << "ecs_test: all assertions passed\n";
    return 0;
}