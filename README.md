# Generic Directed Weighted Graph (GDWG) Library

This C++ library provides a robust implementation of a generic directed weighted graph (GDWG), allowing manipulation and traversal of nodes and edges with optional weights. It supports a wide range of operations, including insertion, deletion, and searches, as well as efficient traversal through custom iterators.

## Overview
The GDWG library is designed to handle complex graph structures with generic node (`N`) and edge (`E`) types. It supports both weighted and unweighted edges, providing flexible mechanisms for graph manipulation and query.

## Features
- **Generic Implementation**: Works with any data types for nodes and edges.
- **Weighted and Unweighted Edges**: Supports both weighted and unweighted edges within the same graph.
- **Comprehensive API**: Includes methods for adding and removing nodes and edges, checking connections, and accessing graph properties.
- **Iterator Support**: Provides iterators for traversing nodes and edges in a graphically structured manner.
- **Exception Safety**: Offers strong exception safety guarantees, ensuring stability across operations.

## Detailed Functionality
### Constructors
- **Default Constructor**: Initializes an empty graph.
- **Initializer List Constructor**: Constructs a graph from an initializer list of nodes.
- **Range Constructor**: Builds a graph from a range of nodes defined by iterators.

### Graph Operations
- **Node Operations**: Insert, replace, and erase nodes.
- **Edge Operations**: Add, modify, and remove edges with optional weights.
- **Graph Queries**: Check connections, retrieve nodes and edges, and perform custom searches using iterators.

### Iterator Functionality
- **Bidirectional Iterators**: Navigate through nodes and their corresponding edges in both directions.
- **Custom Access**: Iterators provide access to node connections and edge properties, supporting complex traversal scenarios.

## Installation
1. Clone the repository:
    ```sh
    git clone git@github.com:shaynewx/GDWG.git
    ```
2. Include the library files from the `src` directory in your project:
   ```
    - `gdwg_graph.h`
    - `gdwg_graph.cpp`
    - `client.cpp`
    ```

3. Compile your project using a C++ compiler that supports C++17 or later.

## Usage Example
To demonstrate the capabilities of the GDWG library, you can compile and run the `client.cpp` example located in the `src` folder:
### Compiling and Running the Client Example
Navigate to your project directory and compile the `client.cpp` using a C++ compiler that supports C++17 or later. Here is an example using `g++`:

1. Configure your project with CMake in the root directory:
    ```sh
    cmake -B build
    ```
2. Build the project:
    ```sh
    cd build
    make
    ```
3. Run the client:
    ```sh
    ./clent
    ```

## Testing
Unit tests are provided in `src/filtered_string_view.test.cpp` to ensure the correctness and efficiency of the library. To run the tests:

1. Configure your project with CMake in the root directory:
    ```sh
    cmake -B build
    ```
2. Build the project:
    ```sh
    cd build
    make
    ```
3. Run the tests:
    ```sh
    ./gdwg_graph_test_exe
    ```

## Contribution

Contributions are welcome! Please fork the repository and submit a pull request for any improvements or bug fixes.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Authors
- **[Shayne Wang](https://github.com/shaynewx)**