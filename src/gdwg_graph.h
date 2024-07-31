#ifndef GDWG_GRAPH_H
#define GDWG_GRAPH_H

#include <initializer_list>
#include <unordered_map>
#include <unordered_set>
#include <iomanip>
#include <optional>
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
			oss << src_ << " -> " << dst_ << " | W | " << std::fixed << std::setprecision(1) << weight_;
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
			for (const auto& node : il) {
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
		auto operator=(graph&& other) noexcept -> graph& = default;

		// 复制构造函数
		graph(graph const& other) = default;

		// 复制赋值运算符
		auto operator=(graph const& other) -> graph& = default;

		// 返回图中节点的数量
		[[nodiscard]] std::size_t node_count() const {
			return nodes_.size();
		}

		// 检查一个特定的节点是否存在于图中
		bool contains(const N& node) const {
			return nodes_.find(node) != nodes_.end();
		}

	 private:
		std::unordered_map<N, std::vector<std::pair<N, E>>> adj_list_; // 节点和边的邻接表
		std::unordered_set<N> nodes_; // 节点的集合
	};
} // namespace gdwg

#endif // GDWG_GRAPH_H
