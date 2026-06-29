#pragma once

class IWidget {
  public:
    virtual ~IWidget() = default;

    virtual void processInput() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void draw() const = 0;
};
