#include "gdwg_graph.h"

#include <catch2/catch.hpp>

TEST_CASE("basic test") {
	// These are commented out right now
	//  because withour your implementation
	//  it will not compile. Uncomment them
	//  once you've done the work
	/*auto g = gdwg::graph<int, std::string>{};
	auto n = 5;
	g.insert_node(n);
	CHECK(g.is_node(n));*/
}

TEST_CASE("Weighted Edge Test Cases", "[weighted_edge]") {
	gdwg::weighted_edge<int, double> edge1(1, 2, 3.5);

	SECTION("Test Print Edge") {
		REQUIRE(edge1.print_edge() == "1 -> 2 | W | 3.5");
	}

	SECTION("Test Is Weighted") {
		REQUIRE(edge1.is_weighted() == true);
	}

	SECTION("Test Get Weight") {
		REQUIRE(edge1.get_weight().has_value());
		REQUIRE(edge1.get_weight().value() == 3.5);
	}

	SECTION("Test Get Nodes") {
		auto nodes = edge1.get_nodes();
		REQUIRE(nodes.first == 1);
		REQUIRE(nodes.second == 2);
	}

	SECTION("Test Equality Operator") {
		gdwg::weighted_edge<int, double> edge2(1, 2, 3.5);
		gdwg::weighted_edge<int, double> edge3(1, 3, 3.5);
		REQUIRE(edge1 == edge2);
		REQUIRE_FALSE(edge1 == edge3);
		REQUIRE(edge1 != edge3);
	}
}

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
