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

// 有权边测试
TEST_CASE("Weighted Edge Test Cases", "[weighted_edge]") {
	gdwg::weighted_edge<int, double> edge1(1, 2, 3.5);

	SECTION("Test Print Edge") {
		REQUIRE(edge1.print_edge() == "1 -> 2 | W | 3.5");
		REQUIRE(gdwg::weighted_edge<int, double>(1, 2, 3.501).print_edge() == "1 -> 2 | W | 3.501"); // 测试精度
	}

	SECTION("Test Is Weighted") {
		REQUIRE(edge1.is_weighted() == true);
	}

	SECTION("Test Get Weight") {
		REQUIRE(edge1.get_weight().has_value());
		REQUIRE(edge1.get_weight().value() == 3.5);
		REQUIRE(gdwg::weighted_edge<int, double>(1, 2, -3.5).get_weight().value() == -3.5); // 测试负权重
	}

	SECTION("Test Get Nodes") {
		auto nodes = edge1.get_nodes();
		REQUIRE(nodes.first == 1);
		REQUIRE(nodes.second == 2);
	}

	SECTION("Test Equality Operator") {
		gdwg::weighted_edge<int, double> edge2(1, 2, 3.5);
		gdwg::weighted_edge<int, double> edge3(1, 3, 3.5);
		gdwg::weighted_edge<int, double> edge4(1, 2, 3.6); // 不同权重
		gdwg::weighted_edge<int, double> edge5(2, 1, 3.5); // 反向边
		REQUIRE(edge1 == edge2);
		REQUIRE_FALSE(edge1 == edge3);
		REQUIRE_FALSE(edge1 == edge4); // 确认权重差异导致不等
		REQUIRE_FALSE(edge1 == edge5); // 确认方向性导致不等
		REQUIRE(edge1 != edge3);
		REQUIRE(edge1 != edge4);
		REQUIRE(edge1 != edge5);
	}
}

// 无权边测试
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

// 图的constructors测试
TEST_CASE("Graph constructor tests", "[graph]") {
	using gdwg::graph;

	SECTION("Default constructor") {
		graph<int, double> g;
		REQUIRE(g.node_count() == 0);
	}

	SECTION("Initializer list constructor") {
		graph<int, double> g{1, 2, 3};
		REQUIRE(g.node_count() == 3);
		REQUIRE(g.contains(1));
		REQUIRE(g.contains(2));
	}

	SECTION("Range constructor") {
		std::vector<int> nodes = {4, 5, 6};
		graph<int, double> g(nodes.begin(), nodes.end());
		REQUIRE(g.node_count() == 3);
		REQUIRE(g.contains(4));
	}

	SECTION("Copy constructor") {
		graph<int, double> g1{1, 2, 3};
		graph<int, double> g2 = g1;
		REQUIRE(g2.node_count() == 3);
		REQUIRE(g2.contains(1));
	}

	SECTION("Move constructor") {
		graph<int, double> g1{1, 2, 3};
		graph<int, double> g2 = std::move(g1);
		REQUIRE(g2.node_count() == 3);
		REQUIRE(g2.contains(1));
		REQUIRE(g1.node_count() == 0);
	}
}

// 图的成员函数测试
TEST_CASE("Graph Tests for graph<std::string, int>", "[graph]") {
	gdwg::graph<std::string, int> g;

	// 测试插入节点
	SECTION("Test insert_node with string identifiers") {
		REQUIRE(g.insert_node("Node1") == true);
		REQUIRE(g.insert_node("Node1") == false);
		REQUIRE(g.insert_node("Node2") == true);
	}

	// 测试插入边
	SECTION("Test insert_edge with string nodes and int weights") {
		g.insert_node("Node1");
		g.insert_node("Node2");

		REQUIRE(g.insert_edge("Node1", "Node2", 10) == true);
		REQUIRE(g.insert_edge("Node1", "Node2", 10) == false);
		REQUIRE(g.insert_edge("Node1", "Node2") == true);
		REQUIRE(g.insert_edge("Node1", "Node2", 20) == true);

		// 尝试在不存在的节点间插入边
		REQUIRE_THROWS_AS(g.insert_edge("Node1", "Node3", 5), std::runtime_error);
	}

	SECTION("Test edge uniqueness between nodes") {
		g.insert_node("Node1");
		g.insert_node("Node2");

		REQUIRE(g.insert_edge("Node1", "Node2", 30) == true);
		REQUIRE(g.insert_edge("Node1", "Node2", 30) == false);
		REQUIRE(g.insert_edge("Node1", "Node2", 40) == true);
	}

	// 测试替换节点
	SECTION("Test replace_node") {
		g.insert_node("Node1");
		g.insert_node("Node2");

		// 成功替换节点
		REQUIRE(g.replace_node("Node1", "Node3") == true); // 替换存在的节点到新节点
		REQUIRE(g.insert_node("Node1") == true); // 替换后原节点名应可重新插入
		REQUIRE(g.insert_node("Node3") == false); // 新节点名已存在
		REQUIRE_THROWS_AS(g.replace_node("Node4", "Node5"), std::runtime_error); // 替换不存在的节点
		REQUIRE(g.replace_node("Node2", "Node3") == false); // Node3 已存在，替换应失败
	}
}