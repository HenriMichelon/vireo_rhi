#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

// Importation de l'exercice
import exo1;
using namespace exo1;

// Exemple de test unitaire
TEST_CASE("Exo1", "[MyClass]") {
    auto obj1 = MyClass{};
    REQUIRE(obj1.GetValue() == 0);
    obj1.SetValue(123);
    REQUIRE(obj1.GetValue() == 123);
    REQUIRE(obj1.ToString() == "123.00");

    const auto obj2 = MyClass{456};
    REQUIRE(obj2.GetValue() == 456);
    REQUIRE(obj2.ToString() == "456.00");

}
