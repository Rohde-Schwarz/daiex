#include "test_iqxformat.h"

namespace IQW
{

using namespace std;

#define RECORDING_NAME "test.iqx"

TEST_F(IqxFormat, Mosaik)
{
#if 0
    Iqx msk(RECORDING_NAME);
    vector<string> arrayNames;
    if(msk.readOpen(arrayNames) != EXIT_FAILURE)
    {
        time_t time = msk.getTimestamp();
        vector<ChannelInfo> channelInfos;
        map<string, string> metadata;
        msk.getMetadata(channelInfos, metadata);

        ASSERT_EQ(arrayNames.at(0), "source1.dat");
        ASSERT_EQ(arrayNames.at(1), "source2.dat");
        ASSERT_EQ(time, 1472479452);
        ASSERT_EQ(msk.getArraySize(arrayNames.at(0)), 123456789);
        ASSERT_EQ(msk.getArraySize(arrayNames.at(1)), 123456789);
        ASSERT_EQ(channelInfos.at(0).getChannelName(), "source1.dat");
        ASSERT_EQ(channelInfos.at(0).getClockRate(), 20000000.000000);
        ASSERT_EQ(channelInfos.at(0).getFrequency(), 98000000.000000);
        ASSERT_EQ(channelInfos.at(0).getSamples(), 0ull);
        ASSERT_EQ(channelInfos.at(1).getChannelName(), "source2.dat");
        ASSERT_EQ(channelInfos.at(1).getClockRate(), 20000000.000000);
        ASSERT_EQ(channelInfos.at(1).getFrequency(), 98000000.000000);
        ASSERT_EQ(channelInfos.at(1).getSamples(), 0ull);
        ASSERT_EQ(metadata["comments"], "test comment");
        ASSERT_EQ(metadata["tags"], "test tag 1test tag 2");
        ASSERT_EQ(metadata["sources"], "source1.datsource2.dat");

       printf("----- Mosaik test -----\n");
       printf("--- streams --- \n");
       for(size_t i=0; i<arrayNames.size(); ++i)
       {
        printf("\t %s \n", arrayNames.at(i).c_str());
       }
       printf("----- time ----- \n");

       tm *ltm = localtime(&time);
       char buffer[32];
       strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ltm);
       printf("\t %s \n", buffer);

       printf("----- Nbr of samples ----- \n");
       for(size_t i=0; i<arrayNames.size(); ++i)
       {
        printf("\t %s: %ld \n", arrayNames.at(i).c_str(), msk->getArraySize(arrayNames.at(i)));
       }

       printf("----- Meta data ----- \n");
       printf("- Channel Information: \n");
       for(size_t i = 0; i < channelInfos.size(); ++i)
       {
        printf("\t name: %s \n", channelInfos.at(i).getChannelName().c_str());
        printf("\t clock: %f \n",channelInfos.at(i).getClockRate());
        printf("\t frequency: %f \n",channelInfos.at(i).getFrequency());
        printf("\t samples: %zu \n",channelInfos.at(i).getSamples());
       }

       printf("- Additional Meta Data Information: \n");
       for(auto it = metadata.cbegin(); it != metadata.cend(); ++it)
       {
        printf("\t %s: %s\n", it->first.c_str(), it->second.c_str());
       }
       printf("-------------------\n");
#endif
}

}// namespace
