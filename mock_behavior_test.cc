#include "_main.skel"
using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

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
    virtual const Boat& getBoatRef() const = 0;
    virtual void getBoatPtr(Boat** boat) const = 0;
    virtual std::string getName() const = 0;
    virtual void getName(std::string& name) const = 0;
};

class MockUser : public User {
public:
    MOCK_METHOD(int, getAge, (), (const, override));
    MOCK_METHOD(bool, isHappy, (), (const, override));
    MOCK_METHOD(User*, getFriend, (), (const, override));
    MOCK_METHOD(Boat, getBoat, (), (const, override));
    MOCK_METHOD(const Boat&, getBoatRef, (), (const, override));
    MOCK_METHOD(void, getBoatPtr, (Boat**), (const, override));
    MOCK_METHOD(std::string, getName, (), (const, override));
    MOCK_METHOD(void, getName, (std::string&), (const, override));
};


TEST(MockBehavior, OnCall_NotCalled) {
    MockUser user{};

    ON_CALL(user, getAge()).WillByDefault(Return(-1));

    // Test passes even though we never call it...
}

TEST(MockBehavior, OnCall_Called) {
    MockUser user{};

    ON_CALL(user, getAge()).WillByDefault(Return(-1));

    // Will always return -1, however many times we call it
    ASSERT_EQ(-1, user.getAge());
    ASSERT_EQ(-1, user.getAge());
    ASSERT_EQ(-1, user.getAge());
}

TEST(MockBehavior, ExpectCall_NotCalled) {
    MockUser user{};

    EXPECT_CALL(user, getAge()).Times(1).WillRepeatedly(Return(1));

    // Test fails because Times(1) expects one call
}

TEST(MockBehavior, ExpectCall_CalledManyTimes) {
    MockUser user{};

    EXPECT_CALL(user, getAge()).WillRepeatedly(Return(1));

    // Can call it any number of times
    ASSERT_EQ(1, user.getAge());
    ASSERT_EQ(1, user.getAge());
    ASSERT_EQ(1, user.getAge());
}

TEST(MockBehavior, ExpectCallWithTimes1_CalledMoreThanOnce) {
    MockUser user{};

    EXPECT_CALL(user, getAge()).WillOnce(Return(1));

    // Test fails because implicit Times(1) expects one call
    ASSERT_EQ(1, user.getAge());
    ASSERT_EQ(0, user.getAge());
    ASSERT_EQ(0, user.getAge());
}

TEST(MockBehavior, ReturnByRef) {
    Boat boat{};
    boat.m_speed = 12;
    MockUser user{};

    // return a reference to `boat
    EXPECT_CALL(user, getBoatRef()).WillOnce(ReturnRef(boat));

    // we got the right boat with our custom speed
    const Boat& boat2 = user.getBoatRef();
    ASSERT_EQ(12, boat2.m_speed);
}

TEST(MockBehavior, SetArgReferee) {
    std::string outarg{"james"};
    MockUser user{};

    // Set the first argument supplied to getName to the value "bill",
    // overwriting its existing value.
    EXPECT_CALL(user, getName(_)).WillOnce(SetArgReferee<0>("bill"));

    user.getName(outarg);
    ASSERT_EQ("bill", outarg);
}

TEST(MockBehavior, SetArgPointee) {
    Boat boat{};
    boat.m_speed = 12;
    MockUser user{};

    // Set the first argument supplied to getBoatPtr to the address of `boat`.
    EXPECT_CALL(user, getBoatPtr(_)).WillOnce(SetArgPointee<0>(&boat));

    // We got our custom boat yay!
    Boat *pboat;
    user.getBoatPtr(&pboat);
    ASSERT_EQ(12, pboat->m_speed);
}