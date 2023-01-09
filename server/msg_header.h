#pragma once

#pragma pack(push, 1)

struct msg_header
{
    int width;
    int height;
    int frame_size;
    int channel;
};

#pragma pack(pop)
