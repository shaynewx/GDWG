#ifndef GDWG_GRAPH_H
#define GDWG_GRAPH_H

// TODO: Make both graph and edge generic
//       ... this won't just compile
//       straight away
namespace gdwg {
	class edge {
	 public:
	 private:
		// You may need to add data members and member functions
		friend class graph;
	};
	class graph {
	 public:
		using edge = gdwg::edge;

		// Your member functions go here
	 private:
	};
} // namespace gdwg

#endif // GDWG_GRAPH_H
