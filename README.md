# LL Games Core System

> A modular C++ core system for building 2D games.

**Work in progress**

## Overview

Template RTS is a modular framework designed to provide a reusable foundation for future 2D games. The project is intended to support several genres, including real-time strategy, turn-based games, and arcade games, while keeping the underlying systems independent and reusable.

The long-term goal is to build a robust core that can be extended with game-specific modules without compromising the reliability of the systems shared by every project.

## Documentation

Documentation is available here: [Documentation](https://sylvain-salviati.com/documentation/ll-core-system)

## Current Features

### Global Systems

- **2D grid** providing the foundation for grid-based operations.
- **Math utilities** covering common operations required by games, including vectors and positions.
- **Pathfinding** featuring an A\* implementation supporting four-directional and eight-directional movement, alongside a Theta\* any-angle pathfinding algorithm.

### RTS Module

- Core system structure built around players, a game master, and entities.
- Initial logic designed with multiplayer compatibility in mind.
- Pathfinding-based unit movement.
- Single-unit and multi-unit selection.
- Formation movement.
- Flag architecture and related gameplay logic.
- Entity attacks.
- Unit pursuit and target tracking.

## In Progress

### Global Systems

- Movement component.

### RTS Module

- Integrate the movement component into unit movement.
- Fix movement and collision issues.
- Make pathfinding account for building collisions.

## Roadmap

### RTS Module

- Refine the flag system and remove flags that are unnecessary or poorly integrated with the rest of the architecture.
- Add building creation.
- Add unit creation.
- Add resource gathering.
- Establish consistent naming conventions, such as choosing between `inTarget` and `target` for method parameters.
- Ensure the architecture remains compatible with multiplayer features.

### Global Systems

- Add an inventory system.
- Add a multiplayer module.
- Add a communication module for integration with Unreal Engine.
- Implement the line-of-sight system with options for turn-based and real time games.

## Development Goals

Because this core system is intended to serve as the foundation for multiple games, robustness is a primary goal. The framework will be continuously refined and subjected to stress testing to verify its behavior across a wide range of situations.
