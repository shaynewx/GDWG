#ifndef GDWG_GRAPH_H
#define GDWG_GRAPH_H

#include <optional>
#include <string>
#include <utility>

// TODO: Make both graph and edge generic
//       ... this won't just compile
//       straight away
namespace gdwg {
	template<typename N, typename E> // 声明template
	// 基类
	class edge {
	 public:
		virtual ~edge() = default; // 用于保证派生类对象可以通过基类指针安全销毁
		[[nodiscard]] virtual std::string print_edge() const = 0; // 用于返回边的字符串表述（ ）
		[[nodiscard]] virtual bool is_weighted() const = 0; // 返回边是否为加权边
		virtual std::optional<E> get_weight() const = 0; // 返回权重（无权边返回std::nullopt）
		virtual std::pair<N, N> get_nodes() const = 0; // 返回边的源节点和目标节点
		virtual bool operator==(const edge& other) const = 0; // 比较两条边是否相等

	 private:
		// You may need to add data members and member functions
		friend class graph;
	};

	// 加权Edge
	template<typename N, typename E>
	class weightwd_edge : public edge<N, E> {
	 public:
		weightwd_edge(const N& src, const N& dst, const E& weight)
		: src_(src)
		, dst_(dst)
		, weight_(weight) {}

	 private:
		N src_;
		N dst_;
		E weight_;
	};

	// 无权Edge
	template<typename N, typename E>
	class unweighted_edge : public edge<N, E> {
	 public:
		unweighted_edge(const N& src, const N& dst)
		: src_(src)
		, dst_(dst) {}

	 private:
		N src_;
		N dst_;
	};

	class graph {
	 public:
		// Your member functions go here
	 private:
	};
} // namespace gdwg

#endif // GDWG_GRAPH_H
