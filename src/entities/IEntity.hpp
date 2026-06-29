#pragma once

class IEntity {
  public:
    virtual ~IEntity() = default;
    virtual void update(float deltaTime) = 0;
    virtual void draw() const = 0;
};
