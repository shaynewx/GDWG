#include "gdwg_graph.h"

#include <catch2/catch.hpp>

TEST_CASE("basic test") {
	// These are commented out right now
	//  because withour your implementation
	//  it will not compile. Uncomment them
	//  once you've done the work
	auto g = gdwg::graph<int, std::string>{};
	auto n = 5;
	g.insert_node(n);
	CHECK(g.is_node(n));
}

// Weighted Edge Test Case
TEST_CASE("Weighted Edge Test Cases", "[weighted_edge]") {
	gdwg::weighted_edge<int, double> edge1(1, 2, 3.5);

	SECTION("Test Print Edge") {
		REQUIRE(edge1.print_edge() == "1 -> 2 | W | 3.5");
		REQUIRE(gdwg::weighted_edge<int, double>(1, 2, 3.501).print_edge() == "1 -> 2 | W | 3.501");
	}

	SECTION("Test Is Weighted") {
		REQUIRE(edge1.is_weighted() == true);
	}

	SECTION("Test Get Weight") {
		REQUIRE(edge1.get_weight().has_value());
		REQUIRE(edge1.get_weight().value() == 3.5);
		REQUIRE(gdwg::weighted_edge<int, double>(1, 2, -3.5).get_weight().value() == -3.5);
	}

	SECTION("Test Get Nodes") {
		auto nodes = edge1.get_nodes();
		REQUIRE(nodes.first == 1);
		REQUIRE(nodes.second == 2);
	}

	SECTION("Test Equality Operator") {
		gdwg::weighted_edge<int, double> edge2(1, 2, 3.5);
		gdwg::weighted_edge<int, double> edge3(1, 3, 3.5);
		gdwg::weighted_edge<int, double> edge4(1, 2, 3.6);
		gdwg::weighted_edge<int, double> edge5(2, 1, 3.5);
		REQUIRE(edge1 == edge2);
		REQUIRE_FALSE(edge1 == edge3);
		REQUIRE_FALSE(edge1 == edge4); // Confirm that weight differences lead to inequality
		REQUIRE_FALSE(edge1 == edge5); // Confirming directionality leads to inequality
		REQUIRE(edge1 != edge3);
		REQUIRE(edge1 != edge4);
		REQUIRE(edge1 != edge5);
	}
}

// Unweighted Edge Test Cases
TEST_CASE("Unweighted Edge Test Cases", "[unweighted_edge]") {
	gdwg::unweighted_edge<int, double> edge1(1, 2);

	SECTION("Test Print Edge") {
		REQUIRE(edge1.print_edge() == "1 -> 2 | U");
	}

	SECTION("Test Is Weighted") {
		REQUIRE_FALSE(edge1.is_weighted());
	}

	SECTION("Test Get Weight") {
		REQUIRE_FALSE(edge1.get_weight().has_value());
	}

	SECTION("Test Get Nodes") {
		auto nodes = edge1.get_nodes();
		REQUIRE(nodes.first == 1);
		REQUIRE(nodes.second == 2);
	}

	SECTION("Test Equality Operator") {
		gdwg::unweighted_edge<int, double> edge2(1, 2);
		gdwg::unweighted_edge<int, double> edge3(2, 1);
		REQUIRE(edge1 == edge2);
		REQUIRE_FALSE(edge1 == edge3);
		REQUIRE(edge1 != edge3);
	}
}

// Graph constructor tests
TEST_CASE("Graph constructor tests", "[graph]") {
	using gdwg::graph;

	SECTION("Default constructor") {
		graph<int, double> g;
		REQUIRE(g.node_count() == 0);
	}

	SECTION("Initializer list constructor") {
		graph<int, double> g{1, 2, 3};
		REQUIRE(g.node_count() == 3);
		REQUIRE(g.is_node(1));
		REQUIRE(g.is_node(2));
	}

	SECTION("Range constructor") {
		std::vector<int> nodes = {4, 5, 6};
		graph<int, double> g(nodes.begin(), nodes.end());
		REQUIRE(g.node_count() == 3);
		REQUIRE(g.is_node(4));
	}

	SECTION("Copy constructor") {
		graph<int, double> g1{1, 2, 3};
		graph<int, double> g2 = g1;
		REQUIRE(g2.node_count() == 3);
		REQUIRE(g2.is_node(1));
	}

	SECTION("Move constructor") {
		graph<int, double> g1{1, 2, 3};
		graph<int, double> g2 = std::move(g1);
		REQUIRE(g2.node_count() == 3);
		REQUIRE(g2.is_node(1));
		REQUIRE(g1.node_count() == 0);
	}
}

// Graph constructor test cases
TEST_CASE("Graph constructor tests with different type of graph", "[graph]") {
	// move constructor
	SECTION("Test move constructor") {
		gdwg::graph<std::string, int> g1;
		g1.insert_node("Node1");
		g1.insert_node("Node2");
		g1.insert_edge("Node1", "Node2", 10);

		gdwg::graph<std::string, int> g2(std::move(g1));
		REQUIRE(g2.is_node("Node1") == true);
		REQUIRE(g2.is_node("Node2") == true);
		REQUIRE(g2.is_connected("Node1", "Node2") == true);
		REQUIRE(g1.empty() == true); // g1 has been cleared
	}

	// move assignment operator
	SECTION("Test move assignment operator") {
		gdwg::graph<std::string, int> g1;
		g1.insert_node("Node1");
		g1.insert_node("Node2");
		g1.insert_edge("Node1", "Node2", 10);

		gdwg::graph<std::string, int> g2;
		g2 = std::move(g1);
		REQUIRE(g2.is_node("Node1") == true);
		REQUIRE(g2.is_node("Node2") == true);
		REQUIRE(g2.is_connected("Node1", "Node2") == true);
		REQUIRE(g1.empty() == true); // g1 has been cleared
	}

	// copy constructor
	SECTION("Test copy constructor") {
		gdwg::graph<std::string, int> g1;
		g1.insert_node("Node1");
		g1.insert_node("Node2");
		g1.insert_edge("Node1", "Node2", 10);

		gdwg::graph<std::string, int> g2(g1);
		REQUIRE(g2.is_node("Node1") == true);
		REQUIRE(g2.is_node("Node2") == true);
		REQUIRE(g2.is_connected("Node1", "Node2") == true);

		// The original graph has not been modified
		REQUIRE(g1.is_node("Node1") == true);
		REQUIRE(g1.is_node("Node2") == true);
		REQUIRE(g1.is_connected("Node1", "Node2") == true);
	}

	// copy assignment operator
	SECTION("Test copy assignment operator") {
		gdwg::graph<std::string, int> g1;
		g1.insert_node("Node1");
		g1.insert_node("Node2");
		g1.insert_edge("Node1", "Node2", 10);

		gdwg::graph<std::string, int> g2;
		g2 = g1;
		REQUIRE(g2.is_node("Node1") == true);
		REQUIRE(g2.is_node("Node2") == true);
		REQUIRE(g2.is_connected("Node1", "Node2") == true);

		// The original graph has not been modified
		REQUIRE(g1.is_node("Node1") == true);
		REQUIRE(g1.is_node("Node2") == true);
		REQUIRE(g1.is_connected("Node1", "Node2") == true);
	}
}

// Test cases for member functions of graph
TEST_CASE("Graph Tests for graph<std::string, int>", "[graph]") {
	gdwg::graph<std::string, int> g;

	// Test insert_node
	SECTION("Test insert_node with string identifiers") {
		REQUIRE(g.insert_node("Node1") == true);
		REQUIRE(g.insert_node("Node1") == false);
		REQUIRE(g.insert_node("Node2") == true);
	}

	// Test insert_edge
	SECTION("Test insert_edge with string nodes and int weights") {
		g.insert_node("Node1");
		g.insert_node("Node2");

		REQUIRE(g.insert_edge("Node1", "Node2", 10) == true);
		REQUIRE(g.insert_edge("Node1", "Node2", 10) == false);
		REQUIRE(g.insert_edge("Node1", "Node2") == true);
		REQUIRE(g.insert_edge("Node1", "Node2", 20) == true);

		// Test insert an edge between non-existent nodes
		REQUIRE_THROWS_AS(g.insert_edge("Node1", "Node3", 5), std::runtime_error);
	}

	SECTION("Test edge uniqueness between nodes") {
		g.insert_node("Node1");
		g.insert_node("Node2");

		REQUIRE(g.insert_edge("Node1", "Node2", 30) == true);
		REQUIRE(g.insert_edge("Node1", "Node2", 30) == false);
		REQUIRE(g.insert_edge("Node1", "Node2", 40) == true);
	}

	// Test replace_node
	SECTION("Test replace_node") {
		g.insert_node("Node1");
		g.insert_node("Node2");

		g.insert_edge("Node1", "Node2", 10);

		// Throw an exception because node does not exist
		REQUIRE_THROWS_AS(g.is_connected("Node1", "Node3"), std::runtime_error);
		REQUIRE_THROWS_AS(g.is_connected("Node3", "Node2"), std::runtime_error);

		// Successfully replaced the node
		REQUIRE(g.replace_node("Node1", "Node3") == true);
		REQUIRE(g.insert_node("Node1") == true);
		REQUIRE(g.insert_node("Node3") == false);
		REQUIRE_THROWS_AS(g.replace_node("Node4", "Node5"), std::runtime_error);
		REQUIRE(g.replace_node("Node2", "Node3") == false);
	}

	// Test merge_replace_node
	SECTION("Test merge_replace_node") {
		g.insert_node("A");
		g.insert_node("B");
		g.insert_node("C");
		g.insert_edge("A", "B", 1);
		g.insert_edge("A", "C", 2);
		g.insert_edge("B", "B", 1);
		g.insert_edge("A", "B");
		g.insert_edge("A", "B", 5);

		// Check if the edge exists
		REQUIRE(g.is_connected("A", "B") == true);
		REQUIRE(g.is_connected("A", "B") == true);

		// Check node and edge migration
		REQUIRE_NOTHROW(g.merge_replace_node("A", "B"));
		REQUIRE(g.is_node("A") == false);
		REQUIRE(g.is_node("B") == true);
		REQUIRE(g.node_count() == 2);

		// Check whether the edges of node B are migrated correctly
		REQUIRE(g.is_connected("B", "B") == true);
		REQUIRE(g.is_connected("B", "C") == true);
		REQUIRE(g.is_connected("B", "B") == true);
		REQUIRE(g.is_connected("B", "B") == true);

		// Throw an exception because nodes do not exist
		REQUIRE_THROWS_AS(g.merge_replace_node("A", "D"), std::runtime_error);
		REQUIRE_THROWS_AS(g.merge_replace_node("E", "B"), std::runtime_error);
	}

	// Test erase_node
	SECTION("Test erase_node") {
		g.insert_node("NodeX");
		g.insert_node("NodeY");
		g.insert_edge("NodeX", "NodeY", 100);

		REQUIRE(g.erase_node("NodeX") == true);
		REQUIRE(g.is_node("NodeX") == false);
	}

	// Test erase_edge
	SECTION("Test erase_edge with src, dst, and weight") {
		g.insert_node("NodeA");
		g.insert_node("NodeB");
		g.insert_edge("NodeA", "NodeB", 50);
		g.insert_edge("NodeA", "NodeB");

		REQUIRE(g.erase_edge("NodeA", "NodeB", 50) == true);

		REQUIRE(g.erase_edge("NodeA", "NodeB", std::nullopt) == true);
		REQUIRE(g.is_connected("NodeA", "NodeB") == false);
	}
}

// Check if there are no nodes in the graph
TEST_CASE("Graph empty tests", "[graph]") {
	gdwg::graph<std::string, int> g;

	SECTION("Check empty on a newly created graph") {
		REQUIRE(g.empty() == true);
	}

	SECTION("Check not empty after adding nodes") {
		g.insert_node("Node1");
		REQUIRE(g.empty() == false);
	}

	SECTION("Check empty after removing all nodes") {
		g.insert_node("Node1");
		g.erase_node("Node1");
		REQUIRE(g.empty() == true);
	}
}

// Check and return the vector of all nodes
TEST_CASE("Graph nodes tests", "[graph]") {
	gdwg::graph<std::string, int> g;

	SECTION("Test nodes on an empty graph") {
		auto nodes = g.nodes();
		REQUIRE(nodes.empty() == true);
	}

	SECTION("Test nodes returns all nodes in ascending order") {
		g.insert_node("Charlie");
		g.insert_node("Alice");
		g.insert_node("Bob");

		auto nodes = g.nodes();
		REQUIRE(nodes.size() == 3);
		REQUIRE(nodes[0] == "Alice");
		REQUIRE(nodes[1] == "Bob");
		REQUIRE(nodes[2] == "Charlie");
	}
}

// Test return all edges from src to dst
TEST_CASE("Graph edges function tests", "[graph]") {
	gdwg::graph<int, double> g;
	g.insert_node(1);
	g.insert_node(2);
	g.insert_edge(1, 2, 10.0);
	g.insert_edge(1, 2);
	g.insert_edge(1, 2, 5.0);

	SECTION("Check sorted edges from src to dst") {
		auto edges = g.edges(1, 2);
		REQUIRE(edges.size() == 3);
		REQUIRE_FALSE(edges[0]->is_weighted()); // First should be unweighted
		REQUIRE(edges[1]->is_weighted());
		REQUIRE(edges[1]->get_weight() == 5.0); // Next should be the lighter weighted edge
		REQUIRE(edges[2]->get_weight() == 10.0); // Heaviest weight last
	}

	SECTION("Throws when nodes do not exist") {
		REQUIRE_THROWS_AS(g.edges(2, 3), std::runtime_error);
	}
}

// Test returns all nodes connected to src
TEST_CASE("Test connections method") {
	gdwg::graph<int, double> g;
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_edge(1, 2, 1.5);
	g.insert_edge(1, 3, 2.5);

	SECTION("Check existing connections") {
		auto result = g.connections(1);
		std::vector<int> expected = {2, 3};
		REQUIRE(result == expected);
	}

	SECTION("Check for node with no connections") {
		auto result = g.connections(2);
		std::vector<int> expected = {};
		REQUIRE(result == expected);
	}

	SECTION("Throw exception for non-existing node") {
		REQUIRE_THROWS_AS(g.connections(4), std::runtime_error);
	}
}

// Operator== overload of graph
TEST_CASE("Graph equality operator tests") {
	gdwg::graph<int, double> g1;
	g1.insert_node(1);
	g1.insert_node(2);
	g1.insert_edge(1, 2, 1.5);

	gdwg::graph<int, double> g2;
	g2.insert_node(1);
	g2.insert_node(2);
	g2.insert_edge(1, 2, 1.5);

	SECTION("Graphs with identical nodes and edges") {
		REQUIRE(g1 == g2);
	}

	SECTION("Graphs with same nodes but different edges") {
		g2.insert_edge(2, 1, 0.5);
		REQUIRE(!(g1 == g2));
	}

	SECTION("Graphs with different nodes") {
		g2.insert_node(3);
		REQUIRE(!(g1 == g2));
	}

	SECTION("Empty graphs") {
		gdwg::graph<int, double> empty1, empty2;
		REQUIRE(empty1 == empty2);
	}
}

// Check if it is possible to format and output all nodes and edges of a graph
TEST_CASE("Graph output format test", "[graph][output]") {
	using graph = gdwg::graph<int, int>;
	auto const v = std::vector<std::tuple<int, int, std::optional<int>>>{
	    {4, 1, -4},
	    {3, 2, 2},
	    {2, 4, std::nullopt},
	    {2, 4, 2},
	    {2, 1, 1},
	    {4, 1, std::nullopt},
	    {6, 2, 5},
	    {6, 3, 10},
	    {1, 5, -1},
	    {3, 6, -8},
	    {4, 5, 3},
	    {5, 2, std::nullopt},
	};

	auto g = graph{};
	for (const auto& [from, to, weight] : v) {
		g.insert_node(from);
		g.insert_node(to);
		if (weight.has_value()) {
			g.insert_edge(from, to, weight.value());
		}
		else {
			g.insert_edge(from, to);
		}
	}
	g.insert_node(64);

	auto out = std::ostringstream{};
	out << g;
	auto const expected_output = std::string_view(R"(
1 (
  1 -> 5 | W | -1
)
2 (
  2 -> 1 | W | 1
  2 -> 4 | U
  2 -> 4 | W | 2
)
3 (
  3 -> 2 | W | 2
  3 -> 6 | W | -8
)
4 (
  4 -> 1 | U
  4 -> 1 | W | -4
  4 -> 5 | W | 3
)
5 (
  5 -> 2 | U
)
6 (
  6 -> 2 | W | 5
  6 -> 3 | W | 10
)
64 (
)
)");
	CHECK(out.str() == expected_output);
}

// Test iterator
TEST_CASE("Iterator functionality for graph<int, int>", "[graph]") {
	gdwg::graph<int, int> g;

	// Insert node
	g.insert_node(1);
	g.insert_node(7);
	g.insert_node(12);
	g.insert_node(14);
	g.insert_node(19);
	g.insert_node(21);
	g.insert_node(31);

	// Insert edges
	g.insert_edge(1, 7, 4);
	g.insert_edge(1, 12, 3);
	g.insert_edge(1, 21, 12);
	g.insert_edge(7, 21, 13);
	g.insert_edge(12, 19, 16);
	g.insert_edge(14, 14, 0);
	g.insert_edge(19, 1, 3);
	g.insert_edge(19, 21, 2);
	g.insert_edge(21, 14, 23);
	g.insert_edge(21, 31, 14);

	// Using iterators to traverse a graph
	SECTION("Verify that all edges are correctly inserted and accessible") {
		std::vector<std::tuple<int, int, std::optional<int>>> expected_edges = {{1, 7, 4},
		                                                                        {1, 12, 3},
		                                                                        {1, 21, 12},
		                                                                        {7, 21, 13},
		                                                                        {12, 19, 16},
		                                                                        {14, 14, 0},
		                                                                        {19, 1, 3},
		                                                                        {19, 21, 2},
		                                                                        {21, 14, 23},
		                                                                        {21, 31, 14}};
		std::vector<std::tuple<int, int, std::optional<int>>> actual_edges;
		for (auto edge : g) {
			actual_edges.emplace_back(edge.from, edge.to, edge.weight);
		}
		REQUIRE(std::is_permutation(actual_edges.begin(), actual_edges.end(), expected_edges.begin()));
	}

	SECTION("Iterator increment and content checking") {
		auto it = g.begin();
		REQUIRE(it != g.end());
		auto edge = *it;
		REQUIRE(edge.from == 1);
		REQUIRE(edge.to == 7);
		REQUIRE(edge.weight == 4); // Check the first edge
		++it;
		edge = *it;
		REQUIRE(edge.from == 1);
		REQUIRE(edge.to == 12);
		REQUIRE(edge.weight == 3); // Check the second edge
	}

	SECTION("Iterator decrement") {
		auto it = g.end();
		--it;
		auto edge = *it;
		REQUIRE(edge.from == 21);
		REQUIRE(edge.to == 31);
		REQUIRE(edge.weight == 14); // Check the last edge
		--it;
		edge = *it;
		REQUIRE(edge.from == 21);
		REQUIRE(edge.to == 14);
		REQUIRE(edge.weight == 23); // Check the second to last edge
	}

	SECTION("Comparison of iterator begin and end") {
		auto it_begin = g.begin();
		auto it_end = g.end();
		REQUIRE(it_begin != it_end); // Begin and end should not be the same
		++it_begin;
		--it_end;
		REQUIRE(it_begin != it_end); // The begin after increment and the end after decrement should not be the same
	}

	SECTION("Post-increment and post-decrement functionality") {
		auto it = g.begin();
		auto prev_it = it++;
		REQUIRE((*prev_it).from == (*g.begin()).from);
		REQUIRE((*prev_it).to == (*g.begin()).to);
		REQUIRE((*prev_it).weight == (*g.begin()).weight);

		auto next_it = it--;
		REQUIRE((*next_it).from == (*std::next(g.begin())).from);
		REQUIRE((*next_it).to == (*std::next(g.begin())).to);
		REQUIRE((*next_it).weight == (*std::next(g.begin())).weight);
	}

	SECTION("Equality and inequality of iterators") {
		auto begin = g.begin();
		auto same_as_begin = g.begin();
		auto end = g.end();

		REQUIRE(begin == same_as_begin); // Tests if iterators at the same position are equal
		REQUIRE(begin != end); // Tests whether iterators at different positions are not equal
		same_as_begin++;
		REQUIRE(begin != same_as_begin);
	}
}

// Tests for removing the edge pointing to iterator i
TEST_CASE("Test erase_edge") {
	gdwg::graph<int, int> g;
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);

	g.insert_edge(1, 2, 10);
	g.insert_edge(2, 3, 20);

	auto it = g.begin();
	auto next_it = g.erase_edge(it);

	SECTION("Iterator points to next element after erase") {
		REQUIRE((*next_it).from == 2);
		REQUIRE((*next_it).to == 3);
		REQUIRE((*next_it).weight == 20);
	}

	SECTION("Check if the edge is really removed") {
		REQUIRE(g.is_connected(1, 2) == false);
	}

	SECTION("Erasing last edge should return end") {
		auto last_edge = g.erase_edge(next_it);
		REQUIRE(last_edge == g.end());
	}
}

// Test find a specific edge based on src, dst and weight
TEST_CASE("Graph find method tests", "[find]") {
	gdwg::graph<int, int> g;
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_edge(1, 2, 10);
	g.insert_edge(2, 3, 20);

	SECTION("Find existing weighted edge") {
		auto it = g.find(1, 2, 10);
		REQUIRE(it != g.end());
		auto edge = *it;
		REQUIRE(edge.from == 1);
		REQUIRE(edge.to == 2);
		REQUIRE(edge.weight.has_value());
		REQUIRE(edge.weight.value() == 10);
	}

	SECTION("Find non-existing edge") {
		auto it = g.find(1, 3);
		REQUIRE(it == g.end());
	}

	SECTION("Find edge with incorrect weight") {
		auto it = g.find(1, 2, 5);
		REQUIRE(it == g.end());
	}

	SECTION("Find unweighted edge when weight is std::nullopt") {
		g.insert_edge(3, 1);
		auto it = g.find(3, 1, std::nullopt);
		REQUIRE(it != g.end());
		auto edge = *it;
		REQUIRE(edge.from == 3);
		REQUIRE(edge.to == 1);
		REQUIRE_FALSE(edge.weight.has_value());
	}
}

// Tests for removing all edges between iterators [i, s)
TEST_CASE("Test erase_edge with iterator range", "[erase_edge]") {
	gdwg::graph<int, int> g;
	g.insert_node(1);
	g.insert_node(2);
	g.insert_node(3);
	g.insert_node(4);
	g.insert_edge(1, 2, 100);
	g.insert_edge(2, 3, 200);
	g.insert_edge(3, 4, 300);

	SECTION("Erase a single edge") {
		auto it1 = g.find(1, 2, 100);
		auto it2 = g.find(2, 3, 200);
		auto result = g.erase_edge(it1, it2); // Should erase the edge (1, 2)

		REQUIRE(result == it2);
		REQUIRE(g.is_connected(1, 2) == false);
		REQUIRE(g.is_connected(2, 3) == true);
	}

	SECTION("Erase multiple edges") {
		auto it1 = g.find(1, 2, 100);
		auto it3 = g.find(3, 4, 300);
		auto result = g.erase_edge(it1, it3); // Should erase edges (1, 2) and (2, 3)

		REQUIRE(result == it3);
		REQUIRE(g.is_connected(1, 2) == false);
		REQUIRE(g.is_connected(2, 3) == false);
		REQUIRE(g.is_connected(3, 4) == true);
	}

	SECTION("Erase all edges up to end") {
		auto it1 = g.find(1, 2, 100);
		auto result = g.erase_edge(it1, g.end()); // Erase all edges

		REQUIRE(result == g.end());
		REQUIRE(g.is_connected(1, 2) == false);
		REQUIRE(g.is_connected(2, 3) == false);
		REQUIRE(g.is_connected(3, 4) == false);
	}
}