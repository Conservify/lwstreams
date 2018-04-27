#include "test_streams.h"
#include "counting_reader.h"
#include "pbtest.h"
#include "protocol/test.pb.h"

StreamsSuite::StreamsSuite() {
}

StreamsSuite::~StreamsSuite() {};

void StreamsSuite::SetUp() {};

void StreamsSuite::TearDown() {};

TEST_F(StreamsSuite, BufferedReader) {
    auto buffer = lws::AlignedStorageBuffer<256>{};
    auto writer = lws::DirectWriter{ buffer.toBufferPtr() };

    auto name1 = "Jacob Lewallen";
    auto name2 = "Shah Selbe";

    EXPECT_EQ(writer.write(name1), strlen(name1));
    EXPECT_EQ(writer.write(name2), strlen(name2));

    {
        auto reader = lws::DirectReader{ writer.toBufferPtr() };

        char buffer1[256] = { 0 };
        EXPECT_EQ(strlen(name1), reader.read((uint8_t *)buffer1, (size_t)strlen(name1)));
        ASSERT_STREQ(name1, buffer1);

        char buffer2[256] = { 0 };
        EXPECT_EQ(strlen(name2), reader.read((uint8_t *)buffer2, (size_t)strlen(name2)));
        ASSERT_STREQ(name2, buffer2);

        ASSERT_EQ(-1, reader.read());
    }

    {
        auto reader = lws::DirectReader{ writer.toBufferPtr() };

        char buffer3[256] = { 0 };
        EXPECT_EQ(strlen(name1) + strlen(name2), reader.read((uint8_t *)buffer3, sizeof(buffer3)));

        auto names = "Jacob LewallenShah Selbe";
        ASSERT_STREQ(buffer3, names);
    }
}

/*
TEST_F(StreamsSuite, ConcatenatedReader) {
    auto buffer = lws::AlignedStorageBuffer<256>{};
    auto ptr = buffer.toBufferPtr();

    fk_module_WireMessageQuery message = fk_module_WireMessageQuery_init_default;
    message.type = fk_module_QueryType_QUERY_CAPABILITIES;
    message.queryCapabilities.version = FK_MODULE_PROTOCOL_VERSION;

    auto writer = lws::DirectWriter{ ptr };
    auto queryWriter = lws::ProtoBufMessageWriter{ writer };
    queryWriter.write(fk_module_WireMessageQuery_fields, &message);

    auto buffer1 = lws::AlignedStorageBuffer<256>{};
    auto buffer2 = lws::AlignedStorageBuffer<256>{};
    auto writer1 = lws::DirectWriter{ buffer1.toBufferPtr() };
    auto writer2 = lws::DirectWriter{ buffer2.toBufferPtr() };

    auto reader1 = lws::DirectReader{ writer1.toBufferPtr() };
    auto reader2 = lws::DirectReader{ writer2.toBufferPtr() };
    auto readers = lws::ConcatenatedReader{ &reader1, &reader2 };

    while (true) {
        auto r = readers.read();
        if (r < 0) {
            break;
        }
    }
}
*/

TEST_F(StreamsSuite, CircularStreamsCloseMidRead) {
    auto circularStreams = lws::CircularStreams<lws::RingBufferN<256>>{ };

    auto& reader = circularStreams.getReader();
    auto& writer = circularStreams.getWriter();

    auto name1 = "Jacob";
    auto name2 = "Shah";

    EXPECT_EQ(writer.write(name1), strlen(name1));
    EXPECT_EQ(writer.write(name2), strlen(name2));

    {
        char scratch[256] = { 0 };
        EXPECT_EQ(strlen(name1), reader.read((uint8_t *)scratch, strlen(name1)));
        scratch[strlen(name1)] = 0;
        ASSERT_STREQ(name1, scratch);

        writer.close();

        EXPECT_EQ(strlen(name2), reader.read((uint8_t *)scratch, strlen(name2)));
        scratch[strlen(name2)] = 0;
        ASSERT_STREQ(name2, scratch);

        ASSERT_EQ(-1, reader.read((uint8_t *)scratch, sizeof(scratch)));
    }
}

TEST_F(StreamsSuite, CircularStreamsBasics) {
    auto circularStreams = lws::CircularStreams<lws::RingBufferN<256>>{ };

    auto& reader = circularStreams.getReader();
    auto& writer = circularStreams.getWriter();

    auto name1 = "Jacob";
    auto name2 = "Shah";

    EXPECT_EQ(writer.write(name1), strlen(name1));

    {
        char scratch[256] = { 0 };
        EXPECT_EQ(strlen(name1), reader.read((uint8_t *)scratch, sizeof(scratch)));
        scratch[strlen(name1)] = 0;
        ASSERT_STREQ(name1, scratch);

        ASSERT_EQ(0, reader.read((uint8_t *)scratch, sizeof(scratch)));
    }

    EXPECT_EQ(writer.write(name2), strlen(name2));

    {
        char scratch[256] = { 0 };
        EXPECT_EQ(strlen(name2), reader.read((uint8_t *)scratch, sizeof(scratch)));
        scratch[strlen(name2)] = 0;
        ASSERT_STREQ(name2, scratch);

        EXPECT_EQ(0, reader.read((uint8_t *)scratch, sizeof(scratch)));
    }

    writer.close();

    {
        char scratch[256] = { 0 };
        ASSERT_EQ(-1, reader.read((uint8_t *)scratch, sizeof(scratch)));
    }
}

TEST_F(StreamsSuite, CircularStreamsBufferPtr) {
    auto buffer = lws::AlignedStorageBuffer<8>{};
    auto ptr = buffer.toBufferPtr();

    auto circularStreams = lws::CircularStreams<lws::RingBufferPtr>{ ptr };

    auto& reader = circularStreams.getReader();
    auto& writer = circularStreams.getWriter();

    auto name1 = "Jacob";
    auto name2 = "Shah";

    EXPECT_EQ(writer.write(name1), strlen(name1));

    {
        char scratch[256] = { 0 };
        EXPECT_EQ(strlen(name1), reader.read((uint8_t *)scratch, sizeof(scratch)));
        scratch[strlen(name1)] = 0;
        ASSERT_STREQ(name1, scratch);

        ASSERT_EQ(0, reader.read((uint8_t *)scratch, sizeof(scratch)));
    }

    EXPECT_EQ(writer.write(name2), strlen(name2));

    {
        char scratch[256] = { 0 };
        EXPECT_EQ(strlen(name2), reader.read((uint8_t *)scratch, sizeof(scratch)));
        scratch[strlen(name2)] = 0;
        ASSERT_STREQ(name2, scratch);

        EXPECT_EQ(0, reader.read((uint8_t *)scratch, sizeof(scratch)));
    }

    writer.close();

    {
        char scratch[256] = { 0 };
        ASSERT_EQ(-1, reader.read((uint8_t *)scratch, sizeof(scratch)));
    }
}

TEST_F(StreamsSuite, CircularStreamsFull) {
    auto buffer = lws::AlignedStorageBuffer<4>{};
    auto ptr = buffer.toBufferPtr();

    auto circularStreams = lws::CircularStreams<lws::RingBufferPtr>{ ptr };

    auto& reader = circularStreams.getReader();
    auto& writer = circularStreams.getWriter();

    auto name1 = "Jacob";

    EXPECT_EQ(writer.write(name1), 4);
    EXPECT_EQ(writer.write(name1), 0);

    {
        char scratch[256] = { 0 };
        EXPECT_EQ(4, reader.read((uint8_t *)scratch, sizeof(scratch)));
        scratch[4] = 0;
        ASSERT_STREQ("Jaco", scratch);

        ASSERT_EQ(0, reader.read((uint8_t *)scratch, sizeof(scratch)));
    }
}

TEST_F(StreamsSuite, CircularStreamsProtoRoundTrip) {
    lwstest_Message incoming = lwstest_Message_init_default;
    incoming.message.funcs.decode = pb_decode_string;

    lwstest_Message outgoing = lwstest_Message_init_default;
    outgoing.time = 15687845564;
    outgoing.message.arg = (void *)"abcdefghijklmno";
    outgoing.message.funcs.encode = pb_encode_string;

    auto buffer = lws::AlignedStorageBuffer<64>{};
    auto ptr = buffer.toBufferPtr();
    auto cs = lws::CircularStreams<lws::RingBufferPtr>{ ptr };

    auto& reader = cs.getReader();
    auto& writer = cs.getWriter();

    auto protoWriter = lws::ProtoBufMessageWriter{ writer };
    EXPECT_EQ(protoWriter.write(lwstest_Message_fields, &outgoing), 24);

    writer.close();

    auto protoReader = lws::ProtoBufMessageReader{ reader };

    EXPECT_EQ(protoReader.read<64>(lwstest_Message_fields, &incoming), 24);

    ASSERT_EQ(outgoing.time, incoming.time);
    ASSERT_STREQ((const char *)outgoing.message.arg, (const char *)incoming.message.arg);

    EXPECT_EQ(protoReader.read<64>(lwstest_Message_fields, &incoming), -1);

    // Notice that we're writing to the writer that we closed earlier. I think
    // that eventually this makes more sense if we throw here. Then we can
    // require the pairs of readers/writers to be opened together to allow for
    // this scenario. The problem then because that we need to have "room" for
    // more than one reader/writer. I'm thinking that we can tell the
    // CircularStreams class how many readers/writers to allow, and free them
    // when both ends are closed.
    // To elaborate more... we don't allow multiple readers, so other reads are
    // expected to be closed. The idea is to prevent the re-using of readers
    // across a single operation from confusing things.

    EXPECT_EQ(protoWriter.write(lwstest_Message_fields, &outgoing), 24);

    incoming = lwstest_Message_init_default;
    incoming.message.funcs.decode = pb_decode_string;

    EXPECT_EQ(protoReader.read<64>(lwstest_Message_fields, &incoming), 24);

    incoming = lwstest_Message_init_default;
    incoming.message.funcs.decode = pb_decode_string;

    EXPECT_EQ(protoReader.read<64>(lwstest_Message_fields, &incoming), -1);
}

TEST_F(StreamsSuite, CircularStreamsProtoCounting) {
    auto reader = CountingReader{ 196, 0 };
    auto total = 0;

    while (true) {
        uint8_t buffer[24];
        auto r = reader.read(buffer, sizeof(buffer));
        if (r == lws::Stream::EOS) {
            break;
        }
        total += r;
    }

    EXPECT_EQ(total, 196);
}

TEST_F(StreamsSuite, CircularStreamsProtoCopying) {
    auto destination = lws::AlignedStorageBuffer<256>{};
    auto buffer = lws::AlignedStorageBuffer<256>{};

    auto writer = lws::DirectWriter{ destination.toBufferPtr() };
    auto reader = CountingReader{ 196, 0 };
    auto total = 0;

    auto copier = lws::StreamCopier{ buffer.toBufferPtr() };

    while (true) {
        auto r = copier.copy(reader, writer);
        if (r < 0) {
            break;
        }
        total += r;
    }

    EXPECT_EQ(total, 196);
}

TEST_F(StreamsSuite, CircularStreamsVarintStream) {
    lwstest_Message outgoing = lwstest_Message_init_default;
    outgoing.time = 15687845564;
    outgoing.message.arg = (void *)"abcdefghijklmno";
    outgoing.message.funcs.encode = pb_encode_string;

    auto destination = lws::AlignedStorageBuffer<256>{};
    auto writer = lws::DirectWriter{ destination.toBufferPtr() };

    auto protoWriter = lws::ProtoBufMessageWriter{ writer };
    EXPECT_EQ(protoWriter.write(lwstest_Message_fields, &outgoing), 24);
    EXPECT_EQ(protoWriter.write(lwstest_Message_fields, &outgoing), 24);
    EXPECT_EQ(protoWriter.write(lwstest_Message_fields, &outgoing), 24);
    EXPECT_EQ(protoWriter.write(lwstest_Message_fields, &outgoing), 24);

    {
        auto scratch = lws::AlignedStorageBuffer<256>{};
        auto reader = lws::DirectReader{ writer.toBufferPtr() };
        auto varintReader = lws::VarintEncodedStream{ reader, scratch.toBufferPtr() };

        for (size_t i = 0; i < 4; ++i)
        {
            auto block = varintReader.read();
            EXPECT_EQ(block.totalSize, 23);
            EXPECT_EQ(block.blockSize, 23);
            EXPECT_EQ(block.position, 0);
        }
        {
            auto block = varintReader.read();
            EXPECT_EQ(block.eos(), true);
        }
    }

    {
        auto scratch = lws::AlignedStorageBuffer<32>{};
        auto reader = lws::DirectReader{ writer.toBufferPtr() };
        auto varintReader = lws::VarintEncodedStream{ reader, scratch.toBufferPtr() };

        {
            auto block = varintReader.read();
            EXPECT_EQ(block.totalSize, 23);
            EXPECT_EQ(block.blockSize, 23);
            EXPECT_EQ(block.position, 0);
        }
        {
            auto block = varintReader.read();
            EXPECT_EQ(block.totalSize, 23);
            EXPECT_EQ(block.blockSize, 7);
            EXPECT_EQ(block.position, 0);
        }
        {
            auto block = varintReader.read();
            EXPECT_EQ(block.totalSize, 23);
            EXPECT_EQ(block.blockSize, 16);
            EXPECT_EQ(block.position, 7);
        }
        {
            auto block = varintReader.read();
            EXPECT_EQ(block.totalSize, 23);
            EXPECT_EQ(block.blockSize, 15);
            EXPECT_EQ(block.position, 0);
        }
        {
            auto block = varintReader.read();
            EXPECT_EQ(block.totalSize, 23);
            EXPECT_EQ(block.blockSize, 8);
            EXPECT_EQ(block.position, 15);
        }
        {
            auto block = varintReader.read();
            EXPECT_EQ(block.totalSize, 23);
            EXPECT_EQ(block.blockSize, 23);
            EXPECT_EQ(block.position, 0);
        }
        {
            auto block = varintReader.read();
            EXPECT_EQ(block.eos(), true);
        }
    }

    {
        auto scratch = lws::AlignedStorageBuffer<8>{};
        auto reader = lws::DirectReader{ writer.toBufferPtr() };
        auto varintReader = lws::VarintEncodedStream{ reader, scratch.toBufferPtr() };

        {
            auto block = varintReader.read();
            EXPECT_EQ(block.totalSize, 23);
            EXPECT_EQ(block.blockSize, 7);
            EXPECT_EQ(block.position, 0);
        }
        {
            auto block = varintReader.read();
            EXPECT_EQ(block.totalSize, 23);
            EXPECT_EQ(block.blockSize, 8);
            EXPECT_EQ(block.position, 7);
        }
        {
            auto block = varintReader.read();
            EXPECT_EQ(block.totalSize, 23);
            EXPECT_EQ(block.blockSize, 8);
            EXPECT_EQ(block.position, 15);
        }
        {
            auto block = varintReader.read();
            EXPECT_EQ(block.totalSize, 23);
            EXPECT_EQ(block.blockSize, 7);
            EXPECT_EQ(block.position, 0);
        }
    }
}
