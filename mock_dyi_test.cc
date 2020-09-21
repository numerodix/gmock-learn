#include "_main.skel"
using ::testing::Return;

// We want to be able to test UserManager. However, it depends on a client. In
// order for us to test the manager we need to somehow mock the client out of
// the test.
//
// Helpfully, the client is an interface type here, so the manager is not
// hardcoded to use the real client.

class Client {
public:
    virtual ~Client() = default;
    virtual int getUserAge(std::string) = 0;
    int giveMeOne() { return 1; }
};

class UserManager {
public:
    explicit UserManager(Client *client) : m_client{client} {}

    int combineUserAges(std::string user1, std::string user2) {
        int age1 = m_client->getUserAge(user1);
        int age2 = m_client->getUserAge(user2);
        return age1 + age2;
    }

private:
    Client* m_client;
};


// Okay, so we want to mock a client which makes some kind of request....

class ClientImpl : public Client {
public:
    int getUserAge(std::string user_id) override {
        // something complicated happens in this function

        // make sure the test never actually calls this, so let's just blow up
        // here
        std::abort();
    } 
};

// We can then create a mock client using the same interface.

class MockClient : public Client {
public:
    MOCK_METHOD(int, getUserAge, (std::string), (override));
    // was not meant to be mocked but we can still mock it!
    MOCK_METHOD(int, giveMeOne, (), ());
};


// And now we can write a test against the manager, where the client in use is
// the mock client.

TEST(MockDemo, CallMock) {
    // ClientImpl cli{};  // can't use this one!
    MockClient cli{};  // have to use this one!
    UserManager mgr{&cli};

    // Now, because the client is a mock we have to program its behavior first

    // when called with "bob" the client should return 2
    EXPECT_CALL(cli, getUserAge("bob")).Times(1).WillOnce(Return(2));
    // when called with "bill" the client should return 3
    EXPECT_CALL(cli, getUserAge("bill")).Times(1).WillOnce(Return(3));

    int totalAge = mgr.combineUserAges("bob", "bill");

    EXPECT_EQ(totalAge, 5);

    // we can even call the second mocked method
    EXPECT_CALL(cli, giveMeOne()).WillRepeatedly(Return(1));
    EXPECT_EQ(cli.giveMeOne(), 1);
}