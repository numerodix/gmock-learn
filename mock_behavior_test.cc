#include "_main.skel"
using ::testing::_;
using ::testing::DoAll;
using ::testing::Expectation;
using ::testing::IgnoreResult;
using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::InvokeArgument;
using ::testing::InvokeWithoutArgs;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;
using ::testing::StrictMock;
using ::testing::Throw;
using ::testing::WithArgs;

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
    virtual int getBoatPtr(Boat** boat) const = 0;
    virtual std::string getName() const = 0;
    virtual void getName(std::string& name) const = 0;
    virtual int getSum(int x, int y) const = 0;
    virtual void takeNum(int x) const = 0;
};

class MockUser : public User {
public:
    MOCK_METHOD(int, getAge, (), (const, override));
    MOCK_METHOD(bool, isHappy, (), (const, override));
    MOCK_METHOD(User*, getFriend, (), (const, override));
    MOCK_METHOD(Boat, getBoat, (), (const, override));
    MOCK_METHOD(const Boat&, getBoatRef, (), (const, override));
    MOCK_METHOD(int, getBoatPtr, (Boat**), (const, override));
    MOCK_METHOD(std::string, getName, (), (const, override));
    MOCK_METHOD(void, getName, (std::string&), (const, override));
    MOCK_METHOD(int, getSum, (int, int), (const, override));
    MOCK_METHOD(void, takeNum, (int), (const, override));
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

TEST(MockBehavior, SetArgPointee_And_MultipleActions) {
    Boat boat{};
    boat.m_speed = 12;
    MockUser user{};

    // Set the first argument supplied to getBoatPtr to the address of `boat`.
    // Both set the first argument and return a value.
    EXPECT_CALL(user, getBoatPtr(_)).WillOnce(DoAll(
        SetArgPointee<0>(&boat),
        Return(-2)
    ));

    // We got the custom boat and a return value.
    Boat *pboat;
    ASSERT_EQ(-2, user.getBoatPtr(&pboat));
    ASSERT_EQ(12, pboat->m_speed);
}

TEST(MockBehavior, InvokeFuncAndReturnItsResult) {
    MockUser user{};

    const auto returnOne = []() -> int { return 1; };

    // When called the mock should call a function and return *its* result
    EXPECT_CALL(user, getAge()).WillOnce(InvokeWithoutArgs(returnOne));

    EXPECT_EQ(1, user.getAge());
}

TEST(MockBehavior, InvokeFuncWithArgsPassedToMockedMethod) {
    MockUser user{};

    const auto mult = [](int x, int y) -> int { return x * y; };

    // `getSum` is ostensibly supposed to return a sum of the numbers, but here
    // we are telling it to take the arguments 0th and 1st it is passed, and
    // feed them to the function `mult` instead, and return that as the result
    // of `getSum`.
    EXPECT_CALL(user, getSum(_, _)).WillRepeatedly(WithArgs<0, 1>(mult));

    EXPECT_EQ(6, user.getSum(2, 3));
    EXPECT_EQ(28, user.getSum(7, 4));
}

TEST(MockBehavior, InvokeFuncWithArgsPassedToMockedMethod2) {
    MockUser user{};
    int age = 0;

    const auto setter = [&](int x) -> void { age = x; };

    // `takeNum` should take the 0th argument it was called with, and pass it to
    // the function `setter`. It should return `void`.
    EXPECT_CALL(user, takeNum(_)).WillRepeatedly(WithArgs<0>(Invoke(setter)));

    // This will indirectly call setter(7)
    user.takeNum(7);

    // setter set `age` to 7
    EXPECT_EQ(7, age);
}

ACTION(Mult) { return arg0 * arg1; }

TEST(MockBehavior, InvokeCustomAction) {
    MockUser user{};

    // `getSum` should just call our custom action `Mult`
    EXPECT_CALL(user, getSum(_, _)).WillOnce(Mult());

    EXPECT_EQ(6, user.getSum(2, 3));
}

TEST(MockBehavior, WillTwice) {
    MockUser user{};

    // `getSum` should just call our custom action `Mult`
    EXPECT_CALL(user, getSum(_, _)).WillOnce(Return(1)).WillOnce(Return(2));

    EXPECT_EQ(1, user.getSum(2, 3));
    EXPECT_EQ(2, user.getSum(2, 3));
}

TEST(MockBehavior, NiceMock) {
    NiceMock<MockUser> user{};

    // there is no uninteresting call warning because NiceMock suppresses it
    EXPECT_EQ(0, user.getSum(2, 3));
}

TEST(MockBehavior, StrictMock) {
    StrictMock<MockUser> user{};

    // the uninteresting call warning shows up, and because it's a StrictMock it
    // causes the test to fail!
    EXPECT_EQ(0, user.getSum(2, 3));
}

TEST(MockBehavior, OrderOfCalls_Fails) {
    MockUser user{};

    Expectation fst = EXPECT_CALL(user, getName());
    EXPECT_CALL(user, getAge()).After(fst);

    // The test fails because we called getAge before calling getName first!
    user.getAge();
}

TEST(MockBehavior, OrderOfCalls_Passes) {
    MockUser user{};

    Expectation fst = EXPECT_CALL(user, getName());
    EXPECT_CALL(user, getAge()).After(fst);

    // Test passes because we make the two calls in the expected order
    user.getName();
    user.getAge();
}

TEST(MockBehavior, OrderOfCalls_InSequence_Fails) {
    InSequence s;

    MockUser user{};

    EXPECT_CALL(user, getName());
    EXPECT_CALL(user, getAge());

    // Fails because the calls are not in the order expected above ^
    user.getAge();
    user.getName();
}

TEST(MockBehavior, OrderOfCalls_InSequence_Passes) {
    InSequence s;

    MockUser user{};

    EXPECT_CALL(user, getName());
    EXPECT_CALL(user, getAge());

    // Passes because the calls ARE in the order expected above ^
    user.getName();
    user.getAge();
}