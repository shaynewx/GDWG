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

// 图的构造函数测试
TEST_CASE("Constructors", "[graph]") {
	// 移动构造函数测试
	SECTION("Test move constructor") {
		gdwg::graph<std::string, int> g1;
		g1.insert_node("Node1");
		g1.insert_node("Node2");
		g1.insert_edge("Node1", "Node2", 10);

		gdwg::graph<std::string, int> g2(std::move(g1)); // 移动构造函数
		REQUIRE(g2.is_node("Node1") == true);
		REQUIRE(g2.is_node("Node2") == true);
		REQUIRE(g2.is_connected("Node1", "Node2") == true);
		REQUIRE(g1.empty() == true); // g1 已被清空
	}

	// 移动赋值运算符测试
	SECTION("Test move assignment operator") {
		gdwg::graph<std::string, int> g1;
		g1.insert_node("Node1");
		g1.insert_node("Node2");
		g1.insert_edge("Node1", "Node2", 10);

		gdwg::graph<std::string, int> g2;
		g2 = std::move(g1); // 移动赋值运算符
		REQUIRE(g2.is_node("Node1") == true);
		REQUIRE(g2.is_node("Node2") == true);
		REQUIRE(g2.is_connected("Node1", "Node2") == true);
		REQUIRE(g1.empty() == true); // g1 已被清空
	}

	// 复制构造函数
	SECTION("Test copy constructor") {
		gdwg::graph<std::string, int> g1;
		g1.insert_node("Node1");
		g1.insert_node("Node2");
		g1.insert_edge("Node1", "Node2", 10);

		gdwg::graph<std::string, int> g2(g1); // 复制构造函数
		REQUIRE(g2.is_node("Node1") == true);
		REQUIRE(g2.is_node("Node2") == true);
		REQUIRE(g2.is_connected("Node1", "Node2") == true);

		// 原始图未被修改
		REQUIRE(g1.is_node("Node1") == true);
		REQUIRE(g1.is_node("Node2") == true);
		REQUIRE(g1.is_connected("Node1", "Node2") == true);
	}

	// 复制赋值运算符测试
	SECTION("Test copy assignment operator") {
		gdwg::graph<std::string, int> g1;
		g1.insert_node("Node1");
		g1.insert_node("Node2");
		g1.insert_edge("Node1", "Node2", 10);

		gdwg::graph<std::string, int> g2;
		g2 = g1; // 复制赋值运算符
		REQUIRE(g2.is_node("Node1") == true);
		REQUIRE(g2.is_node("Node2") == true);
		REQUIRE(g2.is_connected("Node1", "Node2") == true);

		// 原始图未被修改
		REQUIRE(g1.is_node("Node1") == true);
		REQUIRE(g1.is_node("Node2") == true);
		REQUIRE(g1.is_connected("Node1", "Node2") == true);
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

		g.insert_edge("Node1", "Node2", 10);

		// 期待抛出异常，因为 "Node3" 不存在
		REQUIRE_THROWS_AS(g.is_connected("Node1", "Node3"), std::runtime_error);
		REQUIRE_THROWS_AS(g.is_connected("Node3", "Node2"), std::runtime_error);

		// 成功替换节点
		REQUIRE(g.replace_node("Node1", "Node3") == true); // 替换存在的节点到新节点
		REQUIRE(g.insert_node("Node1") == true); // 替换后原节点名应可重新插入
		REQUIRE(g.insert_node("Node3") == false); // 新节点名已存在
		REQUIRE_THROWS_AS(g.replace_node("Node4", "Node5"), std::runtime_error); // 替换不存在的节点
		REQUIRE(g.replace_node("Node2", "Node3") == false); // Node3 已存在，替换应失败
	}

	// 测试merge_replace_node
	SECTION("Test merge_replace_node") {
		g.insert_node("A");
		g.insert_node("B");
		g.insert_node("C");
		g.insert_edge("A", "B", 1);
		g.insert_edge("A", "C", 2);
		g.insert_edge("B", "B", 1); // 将成为重复边
		g.insert_edge("A", "B"); // 无权边
		g.insert_edge("A", "B", 5); // 加权边

		// 检查边是否正确存在
		REQUIRE(g.is_connected("A", "B") == true);
		REQUIRE(g.is_connected("A", "B") == true);

		// 正常合并，检查节点和边的迁移
		REQUIRE_NOTHROW(g.merge_replace_node("A", "B"));
		REQUIRE(g.is_node("A") == false); // A节点应该被删除
		REQUIRE(g.is_node("B") == true); // B节点仍然存在
		REQUIRE(g.node_count() == 2); // 总节点数为2（B, C）

		// 检查B节点的边是否正确迁移
		REQUIRE(g.is_connected("B", "B") == true);
		REQUIRE(g.is_connected("B", "C") == true);
		REQUIRE(g.is_connected("B", "B") == true);
		REQUIRE(g.is_connected("B", "B") == true);

		// 尝试合并不存在的节点
		REQUIRE_THROWS_AS(g.merge_replace_node("A", "D"), std::runtime_error);
		REQUIRE_THROWS_AS(g.merge_replace_node("E", "B"), std::runtime_error);
	}

	// 测试删除节点
	SECTION("Test erase_node") {
		g.insert_node("NodeX");
		g.insert_node("NodeY");
		g.insert_edge("NodeX", "NodeY", 100);

		REQUIRE(g.erase_node("NodeX") == true); // 删除存在的节点
		REQUIRE(g.is_node("NodeX") == false); // 确认节点已删除
	}

	// 测试删除边
	SECTION("Test erase_edge with src, dst, and weight") {
		g.insert_node("NodeA");
		g.insert_node("NodeB");
		g.insert_edge("NodeA", "NodeB", 50);
		g.insert_edge("NodeA", "NodeB");

		REQUIRE(g.erase_edge("NodeA", "NodeB", 50) == true); // 删除指定权重的边

		REQUIRE(g.erase_edge("NodeA", "NodeB", std::nullopt) == true); // 删除无权边
		REQUIRE(g.is_connected("NodeA", "NodeB") == false);
		// REQUIRE(g.erase_edge("NodeA", "NodeC", 100) == false); // 尝试删除不存在的边，应返回 false
	}
}

// 检查图中是否没有节点
TEST_CASE("Graph empty tests", "[graph]") {
	gdwg::graph<std::string, int> g;

	SECTION("Check empty on a newly created graph") {
		REQUIRE(g.empty() == true); // 新创建的图应该是空的
	}

	SECTION("Check not empty after adding nodes") {
		g.insert_node("Node1");
		REQUIRE(g.empty() == false); // 添加节点后，图不应该是空的
	}

	SECTION("Check empty after removing all nodes") {
		g.insert_node("Node1");
		g.erase_node("Node1");
		REQUIRE(g.empty() == true); // 删除所有节点后，图应该是空的
	}
}

// 检查返回所有nodes的vector
TEST_CASE("Graph nodes tests", "[graph]") {
	gdwg::graph<std::string, int> g;

	SECTION("Test nodes on an empty graph") {
		auto nodes = g.nodes();
		REQUIRE(nodes.empty() == true); // 对空图调用 nodes() 应该返回空向量
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

// 测试返回src到dst所有边
TEST_CASE("Graph edges function tests", "[graph]") {
	gdwg::graph<int, double> g;
	g.insert_node(1);
	g.insert_node(2);
	g.insert_edge(1, 2, 10.0); // Weighted edge
	g.insert_edge(1, 2); // Unweighted edge
	g.insert_edge(1, 2, 5.0); // Another weighted edge

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

// 测试返回所有连接到src的nodes
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

// 图的operator==重载
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
		REQUIRE(g1 == g2); // 两个图相同
	}

	SECTION("Graphs with same nodes but different edges") {
		g2.insert_edge(2, 1, 0.5);
		REQUIRE(!(g1 == g2)); // 两个图不相同
	}

	SECTION("Graphs with different nodes") {
		g2.insert_node(3);
		REQUIRE(!(g1 == g2)); // 两个图不相同
	}

	SECTION("Empty graphs") {
		gdwg::graph<int, double> empty1, empty2;
		REQUIRE(empty1 == empty2); // 两个空图相同
	}
}

// 检测是否可以格式化输出一个图的所有节点和边
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

// 测试iterator
