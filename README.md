# Activity Manager

Activity Manager is a robust and user-friendly desktop application built in C++ and Qt, designed to help users efficiently organize their daily tasks. The project was developed for a University course in Object-Oriented Programming and emphasizes clean architecture, scalability, and modern UI design.

The application allows users to create, manage, and track three specific types of activities:
* **Events**: Time-bound activities with a start date, end date, and location.
* **Reminders**: Tasks tied to a specific notification time, featuring customizable repetition frequencies (Daily, Weekly, etc.) and snooze capabilities.
* **To-Dos**: Priority-based tasks without strict chronological constraints.

## Key Features

* **Advanced Filtering & Real-Time Search**: Instantly filter activities by keyword (case-insensitive), activity type, completion status, and custom date ranges using a dedicated drop-down panel.
* **Interactive Notifications**: A background timer monitors Reminders and triggers custom popup alerts when the time is reached, allowing users to complete or snooze the task.
* **Visual Indicators**: Events currently in progress are dynamically highlighted with a blinking red indicator in the main list.
* **Smart To-Do Quick List**: Incomplete To-Dos are automatically extracted and sorted by priority (and alphabetically) in a dedicated sidebar for quick access.
* **Data Persistence**: Save and load your activities locally using JSON format.
* **Keyboard Shortcuts**: Streamlined navigation with shortcuts for quick creation (e.g., `Ctrl+E` for Events, `Ctrl+T` for To-Dos), searching (`Ctrl+F`), and saving/loading.

## Architecture & Technical Highlights

The codebase strictly adheres to Object-Oriented Programming principles, enforcing a clear separation between the logical data model and the Graphical User Interface (Model-View pattern). 

Key architectural choices include:
* **Polymorphism**: Extensively used to handle different activity types dynamically without hardcoding their specific behavior in the manager.
* **The Visitor Pattern (Double Dispatch)**: Implemented across three distinct scenarios to maximize decoupling:
  1. **UI Rendering (`UiVisitor`)**: Safely generates specific graphical representations for each activity type without polluting the data model with Qt dependencies.
  2. **JSON Serialization (`JsonSaveVisitor`)**: Allows each concrete class to compile its own JSON block autonomously during the save process.
  3. **Dynamic Form Routing (`FormEditVisitor`)**: Automatically routes the user to the correct editing form within a `QStackedWidget` based on the dynamic type of the selected activity.

## Build Instructions

### Prerequisites
* **C++17** compatible compiler
* **Qt 6** (Core, Gui, Widgets)
* `qmake` or `cmake` (A `.pro` file is provided)

### Standard Compilation (Linux / macOS)

1. Clone the repository:
   ```bash
   git clone https://github.com/monacolama/ActivityManager.git
   cd activity-manager
2. Generate the Makefile using `qmake`:
   ```bash
   qmake ActivityManager.pro
(Note: depending on your system, the command might be `qmake6`)

3. Compile the project:
    ```bash
    make
4. Run the application:
    ```bash
    ./ActivityManager
