#ifndef GDWG_GRAPH_H
#define GDWG_GRAPH_H

#include <initializer_list>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iomanip>
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

// TODO: Make both graph and edge generic
//       ... this won't just compile
//       straight away
namespace gdwg {
	template<typename N, typename E>
	class graph;

	// Edge: An Abstract BASE Class
	template<typename N, typename E>
	class edge {
	 public:
		// Pure virtual function
		virtual ~edge() = default;
		[[nodiscard]] virtual std::string print_edge() const = 0;
		[[nodiscard]] virtual bool is_weighted() const = 0;
		virtual std::optional<E> get_weight() const = 0;
		virtual std::pair<N, N> get_nodes() const = 0;
		virtual bool operator==(const edge& other) const = 0;

	 private:
		friend class graph<N, E>;
	};

	// Class of weighted edges
	template<typename N, typename E>
	class weighted_edge : public edge<N, E> {
	 public:
		// Constructor for weighted edges
		weighted_edge(const N& src, const N& dst, const E& weight)
		: src_(src)
		, dst_(dst)
		, weight_(weight) {}

		// Return a string representation of the edge
		[[nodiscard]] std::string print_edge() const override {
			std::ostringstream oss; // 定义输出字符流
			oss << src_ << " -> " << dst_ << " | W | " << weight_;
			return oss.str();
		}

		// Return whether the edge is a weighted edge (true)
		[[nodiscard]] bool is_weighted() const override {
			return true;
		}

		// Return weight
		std::optional<E> get_weight() const override {
			return weight_;
		}

		// Returns the src and dst nodes of an edge
		std::pair<N, N> get_nodes() const override {
			return {src_, dst_};
		}

		// Compare two edges for equality (operator== overload)
		bool operator==(const edge<N, E>& other) const override {
			auto const* other_edge = dynamic_cast<const weighted_edge<N, E>*>(&other); // Convert other to weighted_edge
			if (other_edge) {
				return this->src_ == other_edge->src_ and this->dst_ == other_edge->dst_
				       and this->weight_ == other_edge->weight_;
			}
			return false;
		}

		// operator!= overload
		bool operator!=(const edge<N, E>& other) const {
			return !(*this == other);
		}

	 private:
		N src_;
		N dst_;
		E weight_;
	};

	// Class of Unweighted edge
	template<typename N, typename E>
	class unweighted_edge : public edge<N, E> {
	 public:
		// Constructor for unweighted edges
		unweighted_edge(const N& src, const N& dst)
		: src_(src)
		, dst_(dst) {}

		// Return a string representation of the edge
		std::string print_edge() const override {
			std::ostringstream oss;
			oss << src_ << " -> " << dst_ << " | U";
			return oss.str();
		}

		// Return whether the edge is a weighted edge (false)
		bool is_weighted() const override {
			return false;
		}

		// Return weight (nullopt)
		std::optional<E> get_weight() const override {
			return std::nullopt;
		}

		// Returns the src and dst nodes of an edge
		std::pair<N, N> get_nodes() const override {
			return {src_, dst_};
		}

		// Compare two edges for equality (operator== overload)
		bool operator==(const edge<N, E>& other) const override {
			auto const* other_edge = dynamic_cast<const unweighted_edge<N, E>*>(&other); // 将other转为unweighted_edge
			if (other_edge) {
				return this->src_ == other_edge->src_ and this->dst_ == other_edge->dst_;
			}
			return false;
		}

		// operator!= overload
		bool operator!=(const edge<N, E>& other) const {
			return !(*this == other);
		}

	 private:
		N src_;
		N dst_;
	};

	// Class of Graph
	template<typename N, typename E>
	class graph {
	 public:
		// Default constructor with noexcept
		graph() noexcept
		: adj_list_{}
		, nodes_{} {}

		// Initializer list constructor, accepts an initializer list as a parameter
		graph(std::initializer_list<N> il) {
			// Clear the state of the current object
			adj_list_.clear();
			nodes_.clear();

			// Inserting elements into the initializer list
			for (auto const& node : il) {
				nodes_.emplace(node);
			}
		}

		// Range Constructor
		template<typename InputIt>
		graph(InputIt first, InputIt last) {
			for (auto it = first; it != last; ++it) {
				nodes_.emplace(*it);
			}
		}

		// Move Constructor
		graph(graph&& other) noexcept = default;

		// Move assignment operator
		graph& operator=(graph&& other) noexcept = default;

		// Copy Constructor
		graph(graph const& other) = default;

		// Copy assignment operator
		graph& operator=(graph const& other) = default;

		// 2.5 Accessors
		// Check if a specific node exists in the graph
		[[nodiscard]] bool is_node(const N& node) const {
			return nodes_.find(node) != nodes_.end();
		}

		// Check if the node exists in the graph (returns true if not)
		[[nodiscard]] bool empty() const {
			return nodes_.empty();
		}

		// Check if there is an edge of a certain weight between two nodes
		[[nodiscard]] bool is_connected(N const& src, N const& dst) const {
			// Check if the src and dst nodes exist, if not, throw an exception
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't exist "
				                         "in the graph");
			}

			const auto& edges = adj_list_.find(src)->second; // Get the edge list of the source node

			// Check if there is an edge to the target node
			for (const auto& edge : edges) {
				if (edge.first == dst) {
					return true; // Return true if a matching edge is found
				}
			}
			return false;
		}

		// 返回图中节点的数量
		[[nodiscard]] std::size_t node_count() const {
			return nodes_.size();
		}

		// 按ascending order返回所有nodes
		[[nodiscard]] std::vector<N> nodes() {
			std::vector<N> result(nodes_.begin(), nodes_.end()); // 直接从 set 复制元素到 vector，元素已经是有序的
			return result; // 返回包含所有节点的向量
		}

		// 返回从src到dst的所有边，并按照指定的排序方式
		[[nodiscard]] std::vector<std::unique_ptr<edge<N, E>>> edges(N const& src, N const& dst) const {
			// 检查 src 和 dst 节点是否存在，如果不存在，抛出异常
			if (!is_node(src) or !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::edges if src or dst node don't exist in the "
				                         "graph");
			}

			// 获取 src 节点的边列表
			const auto& adj_edges = adj_list_.at(src);
			std::vector<std::unique_ptr<edge<N, E>>> edges_list;

			// 遍历边列表，收集目标节点为 dst 的边
			for (const auto& [target, weight] : adj_edges) {
				if (target == dst) {
					if (weight) {
						edges_list.push_back(std::make_unique<weighted_edge<N, E>>(src, dst, *weight));
					}
					else {
						edges_list.push_back(std::make_unique<unweighted_edge<N, E>>(src, dst));
					}
				}
			}

			// 排序：无权重边排在前，有权重边按权重升序排序
			std::sort(edges_list.begin(), edges_list.end(), [](const auto& a, const auto& b) {
				if (!a->is_weighted() and b->is_weighted())
					return true;
				if (a->is_weighted() and !b->is_weighted())
					return false;
				return a->is_weighted() and b->is_weighted() and a->get_weight() < b->get_weight();
			});
			return edges_list;
		}

		// [[nodiscard]] auto find(N const& src, N const& dst, std::optional<E> weight = std::nullopt) -> iterator;

		// 返回从 src 节点出发的所有目标节点，按升序排序
		[[nodiscard]] std::vector<N> connections(N const& src) const {
			// 如果src不存在抛出错误
			if (!is_node(src)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::connections if src doesn't exist in the "
				                         "graph");
			}

			auto it = adj_list_.find(src); // 在邻接表 adj_list_ 中查找 src 节点
			std::set<N> unique_connections; // 存储唯一的目标节点
			if (it != adj_list_.end()) {
				for (const auto& edge : it->second) {
					unique_connections.insert(edge.first); // 每条边的目标节点插入 unique_connections
				}
			}
			// 转换为 std::vector输出
			return std::vector<N>(unique_connections.begin(), unique_connections.end());
		}

		// 2.4 Modifiers
		// 插入node：插入一个新节点
		bool insert_node(const N& value) {
			//  insert 方法返回一个 pair，其中 second 表示是否插入成功
			auto result = nodes_.insert(value);
			if (result.second) { // 如果成功插入了新节点，为该节点创建一个空的边列表
				adj_list_[value] = {};
			}
			return result.second; // 如果节点是新插入的，second 为 true
		}

		// 插入边
		bool insert_edge(N const& src, N const& dst, std::optional<E> weight = std::nullopt) {
			// 检查源节点和目标节点是否存在，如果不存在则抛出异常
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::insert_edge when either src or dst node does "
				                         "not exist");
			}

			// 新的边
			auto new_edge = std::make_pair(dst, weight);

			// 尝试插入边，防止重复
			auto& edges = adj_list_[src];
			if (std::find(edges.begin(), edges.end(), new_edge) != edges.end()) {
				return false; // 如果边已存在则返回false
			}

			// 插入新边
			edges.push_back(new_edge);
			return true;
		}

		// 替换节点 node（用 new_data 替换图中存储的 old_data）
		bool replace_node(N const& old_data, N const& new_data) {
			// 如果存在值为 new_data 的节点，则返回 false
			if (nodes_.find(new_data) != nodes_.end())
				return false;
			// 如果不存在包含 old_data 的节点，则抛出 runtime_error 异常

			if (!is_node(old_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::replace_node on a node that doesn't exist");
			}

			// 替换节点：先删除旧节点，再插入新节点并返回true
			nodes_.erase(old_data);
			nodes_.emplace(new_data);
			return true;
		}

		// merge replace node(将旧节点上的边和权重迁移到新节点上)
		void merge_replace_node(N const& old_data, N const& new_data) {
			// 如果 old_data 或 new_data 不存在于图中，则抛出异常is
			if (!is_node(old_data) || !is_node(new_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node on old or new data if they "
				                         "don't exist in the graph");
			}

			auto& old_edges = adj_list_[old_data];
			auto& new_edges = adj_list_[new_data];

			// 去除重复边
			for (auto& edge : old_edges) {
				auto it = std::find_if(new_edges.begin(), new_edges.end(), [&edge](const auto& existing_edge) {
					return existing_edge.first == edge.first and existing_edge.second == edge.second;
				});

				if (it == new_edges.end()) { // 如果没找到相同的边，才添加
					new_edges.push_back(std::move(edge));
				}
			}

			old_edges.clear();
			nodes_.erase(old_data);
			adj_list_.erase(old_data);
		}

		// 删除所有值为value的节点
		bool erase_node(N const& value) {
			// 找不到该点则返回false
			if (!is_node(value) || !is_node(value)) {
				return false;
			}

			// 从邻接列表中删除与该节点相关的所有边
			adj_list_.erase(value);

			// 遍历图中所有节点的邻接列表
			for (auto& pair : adj_list_) {
				// 访问当前节点的边列表，并删除所有目标节点为 value 的边
				auto& edges = pair.second;
				edges.erase(
				    std::remove_if(edges.begin(), edges.end(), [value](const auto& edge) { return edge.first == value; }),
				    edges.end());
			}
			// 删除节点
			nodes_.erase(nodes_.find(value));
			return true;
		}

		// 删除边（删除表示从 src 到 dst 的边，如果 weight 是 std::nullopt，删除无权边；否则删除具体权重的边）
		bool erase_edge(N const& src, N const& dst, std::optional<E> weight = std::nullopt) {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't exist "
				                         "in the graph");
			}

			auto& edges = adj_list_[src];
			auto before = edges.size();
			edges.erase(
			    std::remove_if(edges.begin(),
			                   edges.end(),
			                   [dst, weight](const auto& edge) { return edge.first == dst and edge.second == weight; }),
			    edges.end());
			return edges.size() != before;
		}

		// 删除所有点
		void clear() noexcept {
			nodes_.clear();
			adj_list_.clear();
		}

		// 2.7 Comparisons 比较两个图是否完全一致(operator==重载)
		[[nodiscard]] bool operator==(graph const& other) const {
			// 节点集合不同则两个图不相等
			if (nodes_ != other.nodes_)
				return false;

			// 邻接列表不同则两个图不相等
			if (adj_list_.size() != other.adj_list_.size())
				return false;

			// 对每个节点检查边的集合
			for (const auto& node : nodes_) {
				if (adj_list_.find(node) == adj_list_.end() or other.adj_list_.find(node) == other.adj_list_.end()) {
					return false; // 有一个图中没有该节点的邻接列表则两个图不相等
				}

				const auto& edges_this = adj_list_.at(node);
				const auto& edges_other = other.adj_list_.at(node);
				if (edges_this != edges_other)
					return false; // 如果有节点的边集合不相等则两个图不相等
			}
			return true;
		}

		// 2.8 输出一个图的所有节点和边
		// 所有节点按照升序排列；每个节点的所有边也需要按顺序输出，首先是无权重的边，然后是有权重的边升序排列
		friend std::ostream& operator<<(std::ostream& os, const graph<N, E>& g) {
			os << '\n'; // 在输出任何节点信息之前加入一个换行符
			std::vector<std::tuple<N, N, std::optional<E>>> edges; // 定义一个向量 edges，用来临时存储和排序所有边的信息

			for (const auto& [node, adj_edges] : g.adj_list_) {
				os << node << " (\n"; // 输出每个节点的名称和一个开括号，表示开始列出与该节点相关的边

				// 清空 edges 向量，然后将当前节点的所有边添加到这个向量中
				edges.clear();
				for (const auto& edge : adj_edges) {
					edges.emplace_back(node, edge.first, edge.second);
				}

				// 首先按目标节点排序，如果目标节点相同，无权边排在有权边前面，最后按权重排序
				std::sort(edges.begin(), edges.end(), [](const auto& lhs, const auto& rhs) {
					if (std::get<1>(lhs) != std::get<1>(rhs))
						return std::get<1>(lhs) < std::get<1>(rhs);
					if (!std::get<2>(lhs).has_value() and std::get<2>(rhs).has_value())
						return true;
					if (std::get<2>(lhs).has_value() and !std::get<2>(rhs).has_value())
						return false;
					return std::get<2>(lhs).value() < std::get<2>(rhs).value();
				});

				// 输出所有排序后的边
				for (const auto& edge : edges) {
					os << "  " << std::get<0>(edge) << " -> " << std::get<1>(edge) << " | "
					   << (std::get<2>(edge) ? "W | " + std::to_string(std::get<2>(edge).value()) : "U") << '\n';
				}
				os << ")\n";
			}
			return os;
		}

		// 2.9 迭代器
		class iterator {
		 public:
			using value_type = struct {
				N from;
				N to;
				std::optional<E> weight;
			};
			using reference = value_type;
			using pointer = void;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::bidirectional_iterator_tag;

			using node_iterator = typename std::map<N, std::vector<std::pair<N, std::optional<E>>>>::iterator;
			using edge_iterator = typename std::vector<std::pair<N, std::optional<E>>>::iterator;

			// 默认构造函数
			iterator()
			: node_it_()
			, edge_it_()
			, graph_ptr_(nullptr) {}

			// 迭代器解引用操作符,返回当前的源节点, 目标节点 和 weight
			reference operator*() {
				return {node_it_->first, edge_it_->first, edge_it_->second};
			}

			// 迭代器遍历
			// 前置递增操作符
			iterator& operator++() {
				++edge_it_; // 增加边迭代器，移动到下一条边
				if (edge_it_ == node_it_->second.end()) {
					++node_it_; // 如果当前节点的所有边都已遍历完，增加节点迭代器，移动到下一个节点
					while (node_it_ != graph_ptr_->adj_list_.end() and node_it_->second.empty()) {
						++node_it_; // 跳过没有边的节点，找到下一个有边的节点
					}
					if (node_it_ != graph_ptr_->adj_list_.end()) {
						edge_it_ = node_it_->second.begin(); // 如果还有节点没有遍历完，则初始化边迭代器
					}
					else {
						edge_it_ = {}; // 确保和end()一致
					}
				}
				return *this;
			}

			// 后置递增操作符
			iterator operator++(int) {
				iterator temp = *this; // 创建当前迭代器的副本
				++(*this); // 调用前置递增操作符
				return temp;
			}

			// 前置递减操作符
			iterator& operator--() {
				if (node_it_ == graph_ptr_->adj_list_.begin() and edge_it_ == node_it_->second.begin()) {
					throw std::out_of_range("Iterator cannot decrement past the beginning of the graph");
				}
				// 如果当前节点在图的末尾或边迭代器在节点的开始，找到前一个有边的节点
				if (node_it_ == graph_ptr_->adj_list_.end() || edge_it_ == node_it_->second.begin()) {
					do {
						--node_it_;
					} while (node_it_->second.empty());
					edge_it_ = node_it_->second.end(); // 将边迭代器指向该节点的最后一条边
				}
				--edge_it_; // 减少边迭代器，移动到前一条边
				return *this;
			}

			// 后置递减操作符
			iterator operator--(int) {
				iterator temp = *this; // 创建当前迭代器的副本
				--(*this); // 调用前置递减操作符
				return temp;
			}

			// 比较操作符
			bool operator==(const iterator& other) const {
				// 比较当前迭代器与另一个迭代器是否相等
				return node_it_ == other.node_it_ and edge_it_ == other.edge_it_ and graph_ptr_ == other.graph_ptr_;
			}

		 private:
			node_iterator node_it_; // 当前节点的迭代器
			edge_iterator edge_it_; // 当前边的迭代器
			const graph* graph_ptr_; // 指向图的指针

			// 构造指向特定元素的迭代器函数
			explicit iterator(node_iterator node_it, edge_iterator edge_it, const graph* graph_ptr)
			: node_it_(node_it)
			, edge_it_(edge_it)
			, graph_ptr_(graph_ptr) {}

			// 友元声明，以便 graph 类可以访问 iterator 类的私有成员
			friend class graph;
		};

		// 2.6 Iterator Access 迭代器访问
		// 返回指向容器中第一个元素的迭代器
		[[nodiscard]] iterator begin() const {
			if (adj_list_.empty()) {
				return end(); // 如果容器为空，begin 应该等同于 end
			}
			// 强制转换去除常量性
			auto node_it = const_cast<std::map<N, std::vector<std::pair<N, std::optional<E>>>>&>(adj_list_).begin();
			return iterator(node_it, node_it->second.begin(), const_cast<graph*>(this));
		}

		// 返回指向列表末尾的迭代器
		[[nodiscard]] iterator end() const {
			// 创建一个表示结束的迭代器，它的节点迭代器指向邻接列表的末尾
			static std::vector<std::pair<N, std::optional<E>>> empty_edge_list; // 用于安全返回 end 迭代器
			// 强制转换去除常量性
			auto node_it = const_cast<std::map<N, std::vector<std::pair<N, std::optional<E>>>>&>(adj_list_).end();
			return iterator(node_it, empty_edge_list.end(), const_cast<graph*>(this));
		}

		// 2.4.7 删除指向迭代器i的边
		iterator erase_edge(iterator i) {
			if (i == end()) {
				return end(); // 如果迭代器已经是end，则直接返回
			}

			auto node_it = i.node_it_;
			auto edge_it = i.edge_it_;

			if (edge_it == node_it->second.end()) {
				return end();
			}

			// 删除当前边并计算下一个元素的位置
			node_it->second.erase(edge_it);
			if (node_it->second.empty()) {
				// 如果当前节点没有更多边，移动到下一个有边的节点
				do {
					++node_it;
					if (node_it == adj_list_.end())
						return end(); // 如果没有更多节点，返回end
				} while (node_it->second.empty());
				return iterator(node_it, node_it->second.begin(), this);
			}
			else {
				// 否则，返回当前节点的下一条边
				if (edge_it != node_it->second.end()) {
					return iterator(node_it, edge_it, this);
				}
				else {
					// 如果是最后一条边被删除，返回下一个节点的第一条边
					return iterator(++node_it, node_it->second.begin(), this);
				}
			}
		}

		// 2.4.8 删除迭代器 [i, s) 之间的所有边
		iterator erase_edge(iterator i, iterator s) {
			while (i != s) {
				auto current = i++;
				erase_edge(current);
			}
			return s;
		}

		// 2.5 返回 指向与指定 src、dst 和权重等价的边的迭代器
		[[nodiscard]] iterator find(N const& src, N const& dst, std::optional<E> weight = std::nullopt) {
			auto src_it = adj_list_.find(src);
			if (src_it == adj_list_.end()) {
				return end(); // 如果不存在源节点也就不存在边，返回end()
			}

			auto& edges = src_it->second;
			for (auto it = edges.begin(); it != edges.end(); ++it) {
				if (it->first == dst and it->second == weight) {
					return iterator(src_it, it, this); // 返回找到匹配的边
				}
			}
			return end(); // 未找到匹配的边，返回 end()
		}

	 private:
		std::map<N, std::vector<std::pair<N, std::optional<E>>>> adj_list_; // 节点和边的邻接表
		std::set<N> nodes_; // 节点的集合
	};
} // namespace gdwg

#endif // GDWG_GRAPH_H
