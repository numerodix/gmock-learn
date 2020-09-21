#include "_main.skel"
using ::testing::NiceMock;

class Boat {
public:
    Boat() = default;
    Boat(int speed) : m_speed{speed} {}

    int m_speed{11};
};

class User {
public:
    virtual ~User() = default;
    virtual int getAge() const = 0;
    virtual bool isHappy() const = 0;
    virtual User* getFriend() const = 0;
    virtual Boat getBoat() const = 0;
    virtual std::string getName() const = 0;
};

class MockUser : public User {
public:
    MOCK_METHOD(int, getAge, (), (const, override));
    MOCK_METHOD(bool, isHappy, (), (const, override));
    MOCK_METHOD(User*, getFriend, (), (const, override));
    MOCK_METHOD(Boat, getBoat, (), (const, override));
    MOCK_METHOD(std::string, getName, (), (const, override));
};


TEST(MockBehavior, NotConfigured) {
    MockUser user{};

    // We did not configure the mock so when we call these methods:
    // - we will get the default values for the type back
    // - we will get "unintersting call" warnings
    ASSERT_EQ(0, user.getAge());
    ASSERT_EQ(false, user.isHappy());
    ASSERT_EQ(nullptr, user.getFriend());

    // If the method is supposed to return a (user defined) type then that type's
    // ctor will be called without arguments.
    Boat boat = user.getBoat();
    ASSERT_EQ(11, boat.m_speed);
    ASSERT_EQ("", user.getName());
}