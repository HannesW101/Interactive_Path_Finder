# Pathfinding Visualizer

A Qt-based application that demonstrates the A\* pathfinding algorithm with interactive visualization.

![Screenshot](https://github.com/user-attachments/assets/46346332-7a4c-4494-a133-95b465ef3c98)
![image](https://github.com/user-attachments/assets/9f0e0899-b5b9-47cf-bc1e-6d9dd8379a19)
![image](https://github.com/user-attachments/assets/b8d11079-8a79-48e3-89b4-31ed82adb170)
![image](https://github.com/user-attachments/assets/62ddb81a-8a96-4a44-845e-2ab77e8f8442)

## Color Key
| Color          | Terrain Type | Movement Cost |
|----------------|--------------|---------------|
| ⬛ Gray    | Wall         | Impassable (∞)    |
| ⬜ White        | Normal       | 1.0           |
| 🟫 Brown        | Rough        | 2.0           |
| 🟨 Yellow   | Boost        | 0.5           |
| 🟩 Green        | Start        | -             |
| 🟥 Red          | Goal         | -             |
| 🔵 Dark Blue Line    | Path         | -             |

## Features

- **Interactive Grid Editing**
  - Place/Remove walls, rough terrain, and boost zones
  - Set start and goal positions
  - Clear grid with one click

- **A\* Algorithm Implementation**
  - Real-time path visualization
  - Terrain cost calculations:
    - Normal: 1.0
    - Rough: 2.0
    - Boost: 0.5
  - Manhattan distance heuristic
  - Optimal pathfinding around obstacles

- **Visualization Tools**
  - Color-coded terrain rendering
  - Smooth animated path drawing
  - Responsive grid scaling (10x10 to 100x100)
  - Clear visual distinction between explored and unexplored areas

- **UI Features**
  - Dedicated tool selection panel
  - Real-time path cost display
  - One-click grid reset
  - Cell type highlighting
