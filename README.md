# PhysicsEngine


## Description

This is a small, basic 2D physics engine created as part of a project for the Sae Institute in Geneva.
It handles forces and collisions detection.

### Features

- Creation of bodies. 
    - Attributes:
        - Position
        - Velocity
        - Mass
    - Dynamic bodies:
        - Forces can be applied to them.
        - Physical collision calculations.
    - Kinematic bodies
         - Not impacted by forces.
         - No physical collisions.
    - Statics bodies
         - Don't move.
         - Physical collision calculations.
- Creation of collider.
    - Attributes
        - Shape
            - Circle
            - Rectangle
            - Polygon (only work for trigger for now)
        - Restitution
        - Friction
        - trigger mode

## Samples

The samples are made with SDL2 and ImGui.

### Planet system
![Planet system](documentation/images/PlanetSystemSample.gif)

### Trigger colliders
![Trigger colliders](documentation/images/TriggerColliderSample.gif)

### Physical collisions
![Physical collisions](documentation/images/CollisionSample.gif)

### Bouncing shapes
![Bouncing shapes](documentation/images/CollisionWithGroundSample.gif)

## Documentation
- [How to build the project ?](documentation/Build.md)
- [Naming convention of the project](documentation/NamingConvention.md)

## Blog posts
- [Optimizations blog post](https://chocolive24.github.io/#)
