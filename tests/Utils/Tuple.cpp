#include <Utils/Tuple.hpp>
#include <catch2/catch.hpp>
#include <tuple>

TEST_CASE("Tuple to member pointers", "[TupleUtil]") {
  struct Base {
    int x = 0xDEADBEEF;
  };
  struct A : public Base {};
  struct B : public Base {};
  struct C : public Base {};
  std::tuple<A, B, C> holder;
  using holder_t = decltype(holder);
  auto pointers  = to_pointers<const Base *>(holder);

  SECTION("Array size should be the same as tuple size") {
    REQUIRE(pointers.size() == std::tuple_size<holder_t>::value);
  };
  SECTION("Pointers should be valid pointers to base type") {
    for(auto ptr : pointers) {
      REQUIRE(ptr != nullptr);
      REQUIRE(ptr->x == 0xDEADBEEF);
    }
  }
  SECTION("Pointers should point to actual instances"){
    REQUIRE(&std::get<0>(holder) == pointers[0]);
    REQUIRE(&std::get<1>(holder) == pointers[1]);
    REQUIRE(&std::get<2>(holder) == pointers[2]);
  }
}