#include <gtest/gtest.h>
#include <chrono>
#include "../../../Deploy/Include/rdk.h"

using namespace RDK;

// Тестовый компонент для проверки свойств
class TestComponent : public UComponent
{
public:
    // Параметр
    UProperty<double, TestComponent, ptPubParameter> Param;
    
    // Состояние
    UProperty<int, TestComponent, ptPubState> State;
    
    // Вход
    UProperty<double, TestComponent, ptPubInput> Input;
    
    // Выход
    UProperty<double, TestComponent, ptPubOutput> Output;

public:
    TestComponent() : 
        Param("Param", this),
        State("State", this),
        Input("Input", this),
        Output("Output", this)
    {
    }

    virtual TestComponent* New(void) { return new TestComponent(); }

protected:
    virtual bool ADefault(void) 
    {
        Param = 10.0;
        State = 0;
        return true;
    }

    virtual bool ABuild(void) { return true; }
    virtual bool AReset(void) { return true; }
    virtual bool ACalculate(void) 
    {
        Output = Input * Param;
        State++;
        return true;
    }
};

// Тест создания и уничтожения свойств
TEST(UPropertyBasic, CreateDestroy)
{
    UStorage storage;
    TestComponent* comp = new TestComponent();
    
    EXPECT_NE(comp, nullptr);
    EXPECT_EQ(comp->Param, 10.0);
    EXPECT_EQ(comp->State, 0);
    
    delete comp;
}

// Тест GetData/SetData для простых типов
TEST(UPropertyBasic, GetSetData)
{
    UStorage storage;
    TestComponent comp;
    
    // Тест параметра
    comp.Param = 42.5;
    EXPECT_DOUBLE_EQ(comp.Param.GetData(), 42.5);
    EXPECT_DOUBLE_EQ(comp.Param(), 42.5);
    
    // Тест состояния
    comp.State = 100;
    EXPECT_EQ(comp.State.GetData(), 100);
    EXPECT_EQ(comp.State(), 100);
    
    // Тест выхода
    comp.Output = 3.14;
    EXPECT_DOUBLE_EQ(comp.Output.GetData(), 3.14);
}

// Тест всех четырех типов свойств
TEST(UPropertyBasic, AllPropertyTypes)
{
    UStorage storage;
    TestComponent comp;
    
    // Параметр
    comp.Param = 1.0;
    EXPECT_DOUBLE_EQ(comp.Param(), 1.0);
    
    // Состояние
    comp.State = 2;
    EXPECT_EQ(comp.State(), 2);
    
    // Вход
    comp.Input = 3.0;
    EXPECT_DOUBLE_EQ(comp.Input(), 3.0);
    
    // Выход
    comp.Output = 4.0;
    EXPECT_DOUBLE_EQ(comp.Output(), 4.0);
}

// Тест производительности доступа (baseline)
TEST(UPropertyPerformance, AccessBaseline)
{
    UStorage storage;
    TestComponent comp;
    comp.Param = 1.0;
    
    const int iterations = 1000000;
    auto start = std::chrono::high_resolution_clock::now();
    
    double sum = 0.0;
    for (int i = 0; i < iterations; ++i)
    {
        sum += comp.Param();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    EXPECT_GT(sum, 0.0);
    // Логируем время для baseline
    std::cout << "Baseline property access: " << duration.count() << " microseconds for " 
              << iterations << " iterations" << std::endl;
}

// Тест операторов
TEST(UPropertyBasic, Operators)
{
    UStorage storage;
    TestComponent comp;
    
    // Оператор присваивания
    comp.Param = 5.0;
    EXPECT_DOUBLE_EQ(comp.Param(), 5.0);
    
    // Оператор приведения типа
    double value = comp.Param;
    EXPECT_DOUBLE_EQ(value, 5.0);
    
    // Оператор вызова функции
    const double& ref = comp.Param();
    EXPECT_DOUBLE_EQ(ref, 5.0);
}

// Тест поиска свойств
TEST(UPropertyBasic, FindProperty)
{
    UStorage storage;
    TestComponent comp;
    
    UEPtr<UIProperty> prop = comp.FindProperty("Param");
    EXPECT_NE(prop.Get(), nullptr);
    
    prop = comp.FindProperty("State");
    EXPECT_NE(prop.Get(), nullptr);
    
    prop = comp.FindProperty("Input");
    EXPECT_NE(prop.Get(), nullptr);
    
    prop = comp.FindProperty("Output");
    EXPECT_NE(prop.Get(), nullptr);
    
    // Несуществующее свойство
    prop = comp.FindProperty("NonExistent");
    EXPECT_EQ(prop.Get(), nullptr);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

