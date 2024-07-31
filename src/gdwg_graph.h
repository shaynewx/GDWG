#ifndef GDWG_GRAPH_H
#define GDWG_GRAPH_H

#include <initializer_list>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iomanip>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// TODO: Make both graph and edge generic
//       ... this won't just compile
//       straight away
namespace gdwg {
	template<typename N, typename E>
	class graph; // 前向声明

	// Edge: An Abstract BASE Class
	template<typename N, typename E>
	class edge {
	 public:
		virtual ~edge() = default; // 用于保证派生类对象可以通过基类指针安全销毁
		virtual std::string print_edge() const = 0; // 用于返回边的字符串表述（ ）
		virtual bool is_weighted() const = 0; // 返回边是否为加权边
		virtual std::optional<E> get_weight() const = 0; // 返回权重（无权边返回std::nullopt）
		virtual std::pair<N, N> get_nodes() const = 0; // 返回边的源节点和目标节点
		virtual bool operator==(const edge& other) const = 0;

	 private:
		friend class graph<N, E>; // 声明模板友元
	};

	// 加权Edge
	template<typename N, typename E>
	class weighted_edge : public edge<N, E> {
	 public:
		// 加权边的构造函数
		weighted_edge(const N& src, const N& dst, const E& weight)
		: src_(src)
		, dst_(dst)
		, weight_(weight) {}

		// 实现纯虚函数
		// 返回边的字符串表述
		std::string print_edge() const override {
			std::ostringstream oss; // 定义输出字符流
			oss << src_ << " -> " << dst_ << " | W | " << weight_;
			return oss.str();
		}

		// 返回边是否为加权边（是）
		bool is_weighted() const override {
			return true;
		}

		// 返回权重
		std::optional<E> get_weight() const override {
			return weight_;
		}

		// 返回边的源节点和目标节点
		std::pair<N, N> get_nodes() const override {
			return {src_, dst_};
		}

		// 比较两条边是否相等 operator==重载
		bool operator==(const edge<N, E>& other) const override {
			auto const* other_edge = dynamic_cast<const weighted_edge<N, E>*>(&other); // 将other转为weighted_edge
			if (other_edge) {
				return this->src_ == other_edge->src_ and this->dst_ == other_edge->dst_
				       and this->weight_ == other_edge->weight_;
			}
			return false;
		}

		// operator!=重载
		bool operator!=(const edge<N, E>& other) const {
			return !(*this == other);
		}

	 private:
		N src_;
		N dst_;
		E weight_;
	};

	// 无权Edge
	template<typename N, typename E>
	class unweighted_edge : public edge<N, E> {
	 public:
		// 无权边的构造函数
		unweighted_edge(const N& src, const N& dst)
		: src_(src)
		, dst_(dst) {}

		// 实现纯虚函数
		// 返回边的字符串表述
		std::string print_edge() const override {
			std::ostringstream oss;
			oss << src_ << " -> " << dst_ << " | U";
			return oss.str();
		}

		// 返回边是否为加权边（否）
		bool is_weighted() const override {
			return false;
		}

		// 返回权重
		std::optional<E> get_weight() const override {
			return std::nullopt;
		}

		// 返回边的源节点和目标节点
		std::pair<N, N> get_nodes() const override {
			return {src_, dst_};
		}

		// 比较两个边是否相等
		bool operator==(const edge<N, E>& other) const override {
			auto const* other_edge = dynamic_cast<const unweighted_edge<N, E>*>(&other); // 将other转为unweighted_edge
			if (other_edge) {
				return this->src_ == other_edge->src_ and this->dst_ == other_edge->dst_;
			}
			return false;
		}

		// operator!=重载
		bool operator!=(const edge<N, E>& other) const {
			return !(*this == other);
		}

	 private:
		N src_;
		N dst_;
	};

	// 图
	template<typename N, typename E>
	class graph {
	 public:
		// 默认构造函数，noexcept防止异常抛出
		graph() noexcept
		: adj_list_{}
		, nodes_{} {}

		// 初始化列表构造函数，接受一个初始化列表作为参数
		graph(std::initializer_list<N> il) {
			// 清空当前对象的状态
			adj_list_.clear();
			nodes_.clear();

			// 插入初始化列表中的元素
			for (auto const& node : il) {
				nodes_.emplace(node);
			}
		}

		// 范围构造函数
		template<typename InputIt>
		graph(InputIt first, InputIt last) {
			for (auto it = first; it != last; ++it) {
				nodes_.emplace(*it);
			}
		}

		// 移动构造函数
		graph(graph&& other) noexcept = default;

		// 移动赋值运算符
		graph& operator=(graph&& other) noexcept = default;

		// 复制构造函数
		graph(graph const& other) = default;

		// 复制赋值运算符
		graph& operator=(graph const& other) = default;

		// 2.5 Accessors
		// 检查一个特定的节点是否存在于图中
		[[nodiscard]] bool is_node(const N& node) const {
			return nodes_.find(node) != nodes_.end();
		}

		// 检查图中是否有节点（没有则返回true）
		[[nodiscard]] bool empty() const {
			return nodes_.empty();
		}

		// 检查两个节点之间是否有特定权重的边
		bool is_connected(const N& src, const N& dst, const std::optional<E>& weight) const {
			// 首先检查源节点是否存在于邻接列表中
			auto it = adj_list_.find(src);
			if (it == adj_list_.end()) {
				return false; // 源节点不存在返回false
			}

			// 获取源节点的边列表
			const auto& edges = it->second;

			// 检查是否存在到目标节点的特定权重的边
			for (const auto& edge : edges) {
				if (edge.first == dst && edge.second == weight) {
					return true; // 找到匹配的边
				}
			}
			return false; // 未找到匹配的边
		}

		// 返回图中节点的数量
		[[nodiscard]] std::size_t node_count() const {
			return nodes_.size();
		}

		// 2.4 Modifiers
		// 插入node：插入一个新节点
		bool insert_node(const N& value) {
			//  insert 方法返回一个 pair，其中 second 表示是否插入成功
			auto result = nodes_.insert(value);
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
					return existing_edge.first == edge.first && existing_edge.second == edge.second;
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
			                   [dst, weight](const auto& edge) { return edge.first == dst && edge.second == weight; }),
			    edges.end());
			return edges.size() != before;
		}

		// 删除所有点
		void clear() noexcept {
			nodes_.clear();
			adj_list_.clear();
		}

	 private:
		std::unordered_map<N, std::vector<std::pair<N, std::optional<E>>>> adj_list_; // 节点和边的邻接表
		std::set<N> nodes_; // 节点的集合
	};
} // namespace gdwg

#endif // GDWG_GRAPH_H
